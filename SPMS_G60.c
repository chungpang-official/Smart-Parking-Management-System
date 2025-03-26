#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_BOOKINGS 100
#define MAX_ESSENTIALS 6
#define MAX_PARKING_SLOTS 10
#define MAX_RESOURCES 3

typedef struct {
    char member[20];
    char date[11];
    char time[6];
    float duration;
    int parkingSlot;
    char essentials[MAX_ESSENTIALS][20];
    char type[20];
    int priority;
    int status;
} Booking;

typedef struct {
    int battery;
    int cable;
    int locker;
    int umbrella;
    int inflation;
    int valet;
} Resources;

Booking bookings[MAX_BOOKINGS];
Resources resources = {MAX_RESOURCES, MAX_RESOURCES, MAX_RESOURCES, 
                      MAX_RESOURCES, MAX_RESOURCES, MAX_RESOURCES};
int totalBookings = 0;

// Function prototypes
void processCommand(char command[]);

int isValidTimeSlot(char date[], char time[]) {
    int year, month, day;
    sscanf(date, "%d-%d-%d", &year, &month, &day);
    if (year != 2025 || month != 5 || day < 10 || day > 16) return 0;

    int hour, minute;
    sscanf(time, "%d:%d", &hour, &minute);
    if (hour < 8 || hour >= 20 || minute != 0) return 0;

    return 1;
}

int checkResources(char essentials[][20], int allocate) {
    int batteryCount = 0, cableCount = 0, lockerCount = 0;
    int umbrellaCount = 0, inflationCount = 0, valetCount = 0;

    // Count requested resources
    for (int i = 0; i < MAX_ESSENTIALS; i++) {
        if (strcmp(essentials[i], "battery") == 0) batteryCount++;
        else if (strcmp(essentials[i], "cable") == 0) cableCount++;
        else if (strcmp(essentials[i], "locker") == 0) lockerCount++;
        else if (strcmp(essentials[i], "umbrella") == 0) umbrellaCount++;
        else if (strcmp(essentials[i], "inflation") == 0) inflationCount++;
        else if (strcmp(essentials[i], "valet") == 0) valetCount++;
    }

    // Check availability
    if (resources.battery < batteryCount || resources.cable < cableCount ||
        resources.locker < lockerCount || resources.umbrella < umbrellaCount ||
        resources.inflation < inflationCount || resources.valet < valetCount) {
        return 0;
    }

    // Allocate if requested
    if (allocate) {
        resources.battery -= batteryCount;
        resources.cable -= cableCount;
        resources.locker -= lockerCount;
        resources.umbrella -= umbrellaCount;
        resources.inflation -= inflationCount;
        resources.valet -= valetCount;
    }

    return 1;
}

void releaseResources(char essentials[][20]) {
    for (int i = 0; i < MAX_ESSENTIALS; i++) {
        if (strcmp(essentials[i], "battery") == 0) resources.battery++;
        else if (strcmp(essentials[i], "cable") == 0) resources.cable++;
        else if (strcmp(essentials[i], "locker") == 0) resources.locker++;
        else if (strcmp(essentials[i], "umbrella") == 0) resources.umbrella++;
        else if (strcmp(essentials[i], "inflation") == 0) resources.inflation++;
        else if (strcmp(essentials[i], "valet") == 0) resources.valet++;
    }
}

int hasSlotConflict(Booking *newBooking) {
    int newHour, newMinute;
    sscanf(newBooking->time, "%d:%d", &newHour, &newMinute);
    int newStart = newHour * 60 + newMinute;
    int newEnd = newStart + (int)(newBooking->duration * 60);
    
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) continue;
        if (strcmp(bookings[i].date, newBooking->date) == 0 &&
            (newBooking->parkingSlot == bookings[i].parkingSlot || newBooking->parkingSlot == -1)) {
            int hour, minute;
            sscanf(bookings[i].time, "%d:%d", &hour, &minute);
            int start = hour * 60 + minute;
            int end = start + (int)(bookings[i].duration * 60);
          
            if (newStart < end && newEnd > start) {
                return 1; // Conflict found
            }
        }
    }
    return 0; // No conflict
}

void addBooking(char member[], char date[], char time[], float duration, 
               int parkingSlot, char essentials[][20], char type[], int priority) {
    if (totalBookings >= MAX_BOOKINGS) {
        printf("Error: Maximum bookings reached\n");
        return;
    }

    if (!isValidTimeSlot(date, time)) {
        printf("Error: Invalid time slot (must be 08:00-20:00, May 10-16, 2025)\n");
        return;
    }
    
    Booking tempBooking;
    strcpy(tempBooking.member, member);
    strcpy(tempBooking.date, date);
    strcpy(tempBooking.time, time);
    tempBooking.duration = duration;
    tempBooking.parkingSlot = parkingSlot;
    tempBooking.status = 1; // Temporary for conflict check

    int slotConflict = hasSlotConflict(&tempBooking);
    int resourcesAvailable = checkResources(essentials, 0);

    Booking *b = &bookings[totalBookings];
    strcpy(b->member, member);
    strcpy(b->date, date);
    strcpy(b->time, time);
    b->duration = duration;
    b->parkingSlot = parkingSlot;

    for (int i = 0; i < MAX_ESSENTIALS; i++) {
        b->essentials[i][0] = '\0';
    }

    int idx = 0;
    for (int i = 0; i < MAX_ESSENTIALS && essentials[i][0] != '\0'; i++) {
        strcpy(b->essentials[idx++], essentials[i]);
    }

    strcpy(b->type, type);
    b->priority = priority;
    b->status = (resourcesAvailable && !slotConflict) ? 1 : 0;

    if (resourcesAvailable && !slotConflict) {
        checkResources(essentials, 1);
    }

    totalBookings++;
    printf("-> [Pending]\n");
}

void addParking(char member[], char date[], char time[], float duration, 
               int parkingSlot, char essentials[][20]) {
    addBooking(member, date, time, duration, parkingSlot, essentials, "Parking", 1);
}

void addReservation(char member[], char date[], char time[], float duration, 
                   int parkingSlot, char essentials[][20]) {
    addBooking(member, date, time, duration, parkingSlot, essentials, "Reservation", 2);
}

void addEvent(char member[], char date[], char time[], float duration, 
             int parkingSlot, char essentials[][20]) {
    addBooking(member, date, time, duration, parkingSlot, essentials, "Event", 3);
}

void bookEssentials(char member[], char date[], char time[], float duration, 
                   char essentials[][20]) {
    addBooking(member, date, time, duration, -1, essentials, "Essentials", 0);
}

void addBatch(char filename[]) {
    char fullFilename[50];
    strcpy(fullFilename, filename);
    FILE *file = fopen(fullFilename, "r");
    if (!file) {
        printf("Error opening file %s\n", fullFilename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        processCommand(line);
    }
    fclose(file);
    printf("-> [Pending]\n");
}

void sortByFCFS() {
    // Already in FCFS order
}

void sortByPriority() {
    // Step 1: Sort bookings by priority in descending order
    for (int i = 0; i < totalBookings - 1; i++) {
        for (int j = 0; j < totalBookings - i - 1; j++) {
            if (bookings[j].priority < bookings[j + 1].priority) {
                Booking temp = bookings[j];
                bookings[j] = bookings[j + 1];
                bookings[j + 1] = temp;
            }
        }
    }

    // Step 2: Reset resources to initial state
    resources.battery = MAX_RESOURCES;
    resources.cable = MAX_RESOURCES;
    resources.locker = MAX_RESOURCES;
    resources.umbrella = MAX_RESOURCES;
    resources.inflation = MAX_RESOURCES;
    resources.valet = MAX_RESOURCES;

    // Step 3: Process bookings in priority order, rejecting conflicts
    for (int i = 0; i < totalBookings; i++) {
        bookings[i].status = 1; // Initially assume accepted
    }

    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) continue; // Skip already rejected bookings

        // Check resource availability
        if (!checkResources(bookings[i].essentials, 1)) {
            bookings[i].status = 0;
            continue;
        }

        // Check for slot conflicts with previously accepted bookings
        int iHour, iMinute;
        sscanf(bookings[i].time, "%d:%d", &iHour, &iMinute);
        int iStart = iHour * 60 + iMinute;
        int iEnd = iStart + (int)(bookings[i].duration * 60);

        for (int j = 0; j < i; j++) {
            if (bookings[j].status == 0) continue; // Skip rejected bookings
            if (strcmp(bookings[i].date, bookings[j].date) == 0 &&
                bookings[i].parkingSlot == bookings[j].parkingSlot &&
                bookings[i].parkingSlot != -1) { // Check slot conflict
                int jHour, jMinute;
                sscanf(bookings[j].time, "%d:%d", &jHour, &jMinute);
                int jStart = jHour * 60 + jMinute;
                int jEnd = jStart + (int)(bookings[j].duration * 60);

                if (iStart < jEnd && iEnd > jStart) {
                    // Conflict found, reject the lower-priority booking (which is j, since i has higher priority)
                    bookings[j].status = 0;
                    releaseResources(bookings[j].essentials);
                }
            }
        }

        // Re-check conflicts after rejecting a booking
        for (int j = i + 1; j < totalBookings; j++) {
            if (bookings[j].status == 0) continue;
            if (strcmp(bookings[i].date, bookings[j].date) == 0 &&
                bookings[i].parkingSlot == bookings[j].parkingSlot &&
                bookings[i].parkingSlot != -1) {
                int jHour, jMinute;
                sscanf(bookings[j].time, "%d:%d", &jHour, &jMinute);
                int jStart = jHour * 60 + jMinute;
                int jEnd = jStart + (int)(bookings[j].duration * 60);

                if (iStart < jEnd && iEnd > jStart) {
                    bookings[j].status = 0;
                    releaseResources(bookings[j].essentials);
                }
            }
        }
    }
}

void sortByOptimized() {
    sortByFCFS();

    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) continue;

        int year, month, day, hour, minute;
        sscanf(bookings[i].date, "%d-%d-%d", &year, &month, &day);
        sscanf(bookings[i].time, "%d:%d", &hour, &minute);

        int startMinutes = hour * 60 + minute;
        int durationMinutes = (int)(bookings[i].duration * 60);
        int endMinutes = startMinutes + durationMinutes;

        resources.battery = MAX_RESOURCES;
        resources.cable = MAX_RESOURCES;
        resources.locker = MAX_RESOURCES;
        resources.umbrella = MAX_RESOURCES;
        resources.inflation = MAX_RESOURCES;
        resources.valet = MAX_RESOURCES;

        for (int j = 0; j < totalBookings; j++) {
            if (j != i && bookings[j].status == 1) {
                checkResources(bookings[j].essentials, 1);
            }
        }

        int found = 0;
        for (int h = hour; h <= 20; h++) {
            int m = (h == hour) ? minute : 0;
            for (; m < 60; m += 60) {
                int newStart = h * 60 + m;
                int newEnd = newStart + durationMinutes;

                if (newEnd > 20 * 60) continue;

                int conflict = 0;
                for (int j = 0; j < totalBookings; j++) {
                    if (j == i || bookings[j].status == 0) continue;
                    if (strcmp(bookings[i].date, bookings[j].date) != 0) continue;
                    if (bookings[i].parkingSlot != bookings[j].parkingSlot) continue;

                    int jHour, jMinute;
                    sscanf(bookings[j].time, "%d:%d", &jHour, &jMinute);
                    int jStart = jHour * 60 + jMinute;
                    int jEnd = jStart + (int)(bookings[j].duration * 60);

                    if (newStart < jEnd && newEnd > jStart) {
                        conflict = 1;
                        break;
                    }
                }

                if (!conflict && checkResources(bookings[i].essentials, 1)) {
                    sprintf(bookings[i].time, "%02d:%02d", h, m);
                    bookings[i].status = 1;
                    found = 1;
                    printf("Rescheduled %s to %s %s, status: %d\n", 
                          bookings[i].member, bookings[i].date, bookings[i].time, bookings[i].status);
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            int nextDay = day;
            int nextMonth = month;
            int nextYear = year;

            while (nextDay < 16) {
                nextDay++;
                if (nextDay > 16) break;

                for (int h = 8; h <= 20; h++) {
                    for (int m = 0; m < 60; m += 60) {
                        int newStart = h * 60 + m;
                        int newEnd = newStart + durationMinutes;

                        if (newEnd > 20 * 60) continue;

                        resources.battery = MAX_RESOURCES;
                        resources.cable = MAX_RESOURCES;
                        resources.locker = MAX_RESOURCES;
                        resources.umbrella = MAX_RESOURCES;
                        resources.inflation = MAX_RESOURCES;
                        resources.valet = MAX_RESOURCES;

                        for (int j = 0; j < totalBookings; j++) {
                            if (j != i && bookings[j].status == 1) {
                                checkResources(bookings[j].essentials, 1);
                            }
                        }

                        int conflict = 0;
                        for (int j = 0; j < totalBookings; j++) {
                            if (j == i || bookings[j].status == 0) continue;
                            int jYear, jMonth, jDay;
                            sscanf(bookings[j].date, "%d-%d-%d", &jYear, &jMonth, &jDay);
                            if (jYear != nextYear || jMonth != nextMonth || jDay != nextDay) continue;
                            if (bookings[i].parkingSlot != bookings[j].parkingSlot) continue;

                            int jHour, jMinute;
                            sscanf(bookings[j].time, "%d:%d", &jHour, &jMinute);
                            int jStart = jHour * 60 + jMinute;
                            int jEnd = jStart + (int)(bookings[j].duration * 60);

                            if (newStart < jEnd && newEnd > jStart) {
                                conflict = 1;
                                break;
                            }
                        }

                        if (!conflict && checkResources(bookings[i].essentials, 1)) {
                            sprintf(bookings[i].date, "%04d-%02d-%02d", nextYear, nextMonth, nextDay);
                            sprintf(bookings[i].time, "%02d:%02d", h, m);
                            bookings[i].status = 1;
                            found = 1;
                            printf("Rescheduled %s to %s %s, status: %d\n", 
                                  bookings[i].member, bookings[i].date, bookings[i].time, bookings[i].status);
                            break;
                        }
                    }
                    if (found) break;
                }
                if (found) break;
            }
        }
    }
}

void writeAcceptedBookings(FILE *file, char algorithm[]) {
    fprintf(file, "\n*** Parking Booking - ACCEPTED / %s ***\n", algorithm);
    char currentMember[20] = "";
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) {
            if (strcmp(currentMember, bookings[i].member) != 0) {
                strcpy(currentMember, bookings[i].member);
                fprintf(file, "\n%s has the following bookings:\n", currentMember);
                fprintf(file, "| %-10s | %-5s | %-5s | %-11s | %-15s |\n", 
                      "Date", "Start", "End", "Type", "Device");
                fprintf(file, "|------------|-------|-------|-------------|-----------------|\n");
            }

            int hour, minute;
            sscanf(bookings[i].time, "%d:%d", &hour, &minute);
            int endHour = hour + (int)bookings[i].duration;
            int endMinute = minute + (int)((bookings[i].duration - (int)bookings[i].duration) * 60);
            if (endMinute >= 60) {
                endHour++;
                endMinute -= 60;
            }
            char endTime[6];
            sprintf(endTime, "%02d:%02d", endHour, endMinute);

            char devices[50] = "";
            for (int j = 0; j < MAX_ESSENTIALS && bookings[i].essentials[j][0] != '\0'; j++) {
                strcat(devices, bookings[i].essentials[j]);
                strcat(devices, " ");
            }

            fprintf(file, "| %-10s | %-5s | %-5s | %-11s | %-15s |\n", 
                  bookings[i].date, bookings[i].time, endTime, 
                  bookings[i].type, devices);
        }
    }
    fprintf(file, "\n- End -\n");
}

void writeRejectedBookings(FILE *file, char algorithm[]) {
    fprintf(file, "\n*** Parking Booking - REJECTED / %s ***\n", algorithm);
    int rejectedCount = 0;

    // Count bookings that are still rejected after all processing
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) {
            rejectedCount++;
        }
    }

    if (rejectedCount == 0) {
        fprintf(file, "\n- End -\n");
        return;
    }

    char currentMember[20] = "";
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) { // Only include bookings that are still rejected
            if (strcmp(currentMember, bookings[i].member) != 0) {
                strcpy(currentMember, bookings[i].member);
                fprintf(file, "\n%s (there are %d bookings rejected):\n", currentMember, rejectedCount);
                fprintf(file, "| %-10s | %-5s | %-5s | %-11s | %-15s |\n", 
                      "Date", "Start", "End", "Type", "Essentials");
                fprintf(file, "|------------|-------|-------|-------------|-----------------|\n");
            }

            int hour, minute;
            sscanf(bookings[i].time, "%d:%d", &hour, &minute);
            int endHour = hour + (int)bookings[i].duration;
            int endMinute = minute + (int)((bookings[i].duration - (int)bookings[i].duration) * 60);
            if (endMinute >= 60) {
                endHour++;
                endMinute -= 60;
            }
            char endTime[6];
            sprintf(endTime, "%02d:%02d", endHour, endMinute);

            char essentials[50] = "";
            for (int j = 0; j < MAX_ESSENTIALS && bookings[i].essentials[j][0] != '\0'; j++) {
                strcat(essentials, bookings[i].essentials[j]);
                strcat(essentials, " ");
            }

            fprintf(file, "| %-10s | %-5s | %-5s | %-11s | %-15s |\n", 
                  bookings[i].date, bookings[i].time, endTime, 
                  bookings[i].type, essentials);
        }
    }
    fprintf(file, "\n- End -\n");
}

void generateSummaryReport() {
    char filename[50];
    sprintf(filename, "SPMS_Report_G60.txt");
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error creating report file\n");
        return;
    }

    int total = totalBookings;
    int invalidRequests = 0;

    int fcfsAccepted = 0, fcfsRejected = 0;
    int prioAccepted = 0, prioRejected = 0;
    int optiAccepted = 0, optiRejected = 0;

    Booking bookingsBackup[MAX_BOOKINGS];
    Resources resourcesBackup = resources;
    memcpy(bookingsBackup, bookings, sizeof(bookings));
    int totalBookingsBackup = totalBookings;

    Resources fcfsResources, prioResources, optiResources;

    // FCFS
    resources = resourcesBackup;
    memcpy(bookings, bookingsBackup, sizeof(bookings));   
    sortByFCFS();
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) fcfsAccepted++;
        else fcfsRejected++;
    }
    fcfsResources = resources;

    // PRIO
    resources = resourcesBackup;
    memcpy(bookings, bookingsBackup, sizeof(bookings));
    sortByPriority();
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) prioAccepted++;
        else prioRejected++;
    }
    prioResources = resources;

    // OPTI
    resources = resourcesBackup;
    memcpy(bookings, bookingsBackup, sizeof(bookings));   
    sortByOptimized();
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) optiAccepted++;
        else optiRejected++;
    }
    optiResources = resources;

    fprintf(file, "*** Parking Booking Manager - Summary Report ***\n\n");
    fprintf(file, "Performance:\n\n");

    fprintf(file, "For FCFS:\n");
    fprintf(file, "    TOTAL NUMBER OF BOOKINGS RECEIVED: %d (%.1f%%)\n", total, 100.0);
    fprintf(file, "    NUMBER OF BOOKINGS ASSIGNED: %d (%.1f%%)\n", fcfsAccepted, (fcfsAccepted * 100.0) / total);
    fprintf(file, "    NUMBER OF BOOKINGS REJECTED: %d (%.1f%%)\n", fcfsRejected, (fcfsRejected * 100.0) / total);
    fprintf(file, "\n    Utilization of TIME SLOT:\n");
    fprintf(file, "        locker - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.locker) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Battery - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.battery) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Umbrella - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.umbrella) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Cable - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.cable) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Inflation - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.inflation) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Valet - %.1f%%\n", ((MAX_RESOURCES - fcfsResources.valet) * 100.0) / MAX_RESOURCES);
    fprintf(file, "\n    Invalid request(s) made: %d\n", invalidRequests);

    fprintf(file, "\nFor PRIO:\n");
    fprintf(file, "    TOTAL NUMBER OF BOOKINGS RECEIVED: %d (%.1f%%)\n", total, 100.0);
    fprintf(file, "    NUMBER OF BOOKINGS ASSIGNED: %d (%.1f%%)\n", prioAccepted, (prioAccepted * 100.0) / total);
    fprintf(file, "    NUMBER OF BOOKINGS REJECTED: %d (%.1f%%)\n", prioRejected, (prioRejected * 100.0) / total);
    fprintf(file, "\n    Utilization of TIME SLOT:\n");
    fprintf(file, "        locker - %.1f%%\n", ((MAX_RESOURCES - prioResources.locker) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Battery - %.1f%%\n", ((MAX_RESOURCES - prioResources.battery) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Umbrella - %.1f%%\n", ((MAX_RESOURCES - prioResources.umbrella) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Cable - %.1f%%\n", ((MAX_RESOURCES - prioResources.cable) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Inflation - %.1f%%\n", ((MAX_RESOURCES - prioResources.inflation) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Valet - %.1f%%\n", ((MAX_RESOURCES - prioResources.valet) * 100.0) / MAX_RESOURCES);
    fprintf(file, "\n    Invalid request(s) made: %d\n", invalidRequests);

    fprintf(file, "\nFor OPTI:\n");
    fprintf(file, "    TOTAL NUMBER OF BOOKINGS RECEIVED: %d (%.1f%%)\n", total, 100.0);
    fprintf(file, "    NUMBER OF BOOKINGS ASSIGNED: %d (%.1f%%)\n", optiAccepted, (optiAccepted * 100.0) / total);
    fprintf(file, "    NUMBER OF BOOKINGS REJECTED: %d (%.1f%%)\n", optiRejected, (optiRejected * 100.0) / total);
    fprintf(file, "\n    Utilization of TIME SLOT:\n");
    fprintf(file, "        locker - %.1f%%\n", ((MAX_RESOURCES - optiResources.locker) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Battery - %.1f%%\n", ((MAX_RESOURCES - optiResources.battery) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Umbrella - %.1f%%\n", ((MAX_RESOURCES - optiResources.umbrella) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Cable - %.1f%%\n", ((MAX_RESOURCES - optiResources.cable) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Inflation - %.1f%%\n", ((MAX_RESOURCES - optiResources.inflation) * 100.0) / MAX_RESOURCES);
    fprintf(file, "        Valet - %.1f%%\n", ((MAX_RESOURCES - optiResources.valet) * 100.0) / MAX_RESOURCES);
    fprintf(file, "\n    Invalid request(s) made: %d\n", invalidRequests);

    fprintf(file, "\n- End of Report -\n");
    fclose(file);
    printf("Report generated in %s\n", filename);
}

void processParallel(char algorithm[]) {
    // Backup bookings and resources
    Booking bookingsBackup[MAX_BOOKINGS];
    Resources resourcesBackup = resources;
    memcpy(bookingsBackup, bookings, sizeof(bookings));
    int totalBookingsBackup = totalBookings;

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child process
        close(fd[0]);
        if (strcmp(algorithm, "prio") == 0 || strcmp(algorithm, "PRIO") == 0) sortByPriority();
        else if (strcmp(algorithm, "fcfs") == 0) sortByFCFS();
        else if (strcmp(algorithm, "opti") == 0) sortByOptimized();

        FILE *file = fopen("temp_accepted.txt", "w");
        if (file) {
            writeAcceptedBookings(file, algorithm);
            fclose(file);
        }

        write(fd[1], "done", 5);
        close(fd[1]);
        exit(0);
    } else { // Parent process
        close(fd[1]);
        char buf[5];
        read(fd[0], buf, 5);

        FILE *file = fopen("temp_rejected.txt", "w");
        if (file) {
            writeRejectedBookings(file, algorithm);
            fclose(file);
        }

        if (strcmp(algorithm, "ALL") == 0) {
            generateSummaryReport();
        }

        close(fd[0]);
        wait(NULL);

        printf("\n");
        char ch;
        FILE *f = fopen("temp_accepted.txt", "r");
        if (f) {
            while ((ch = fgetc(f)) != EOF) putchar(ch);
            fclose(f);
        }

        f = fopen("temp_rejected.txt", "r");
        if (f) {
            while ((ch = fgetc(f)) != EOF) putchar(ch);
            fclose(f);
        }

        remove("temp_accepted.txt");
        remove("temp_rejected.txt");

        // Restore bookings and resources
        memcpy(bookings, bookingsBackup, sizeof(bookings));
        resources = resourcesBackup;
        totalBookings = totalBookingsBackup;
    }
}

void processCommand(char command[]) {
    char cmd[20], member[20], date[11], time[6], essentials[MAX_ESSENTIALS][20] = {""};
    char filename[20], algorithm[10];
    float duration;
    int parkingSlot;

    if (sscanf(command, "%s", cmd) != 1) return;

    if (strcmp(cmd, "addParking") == 0) {
        if (sscanf(command, "%*s -%s %s %s %f %d %s %s", member, date, time, &duration, &parkingSlot, essentials[0], essentials[1]) >= 6) {
            addParking(member, date, time, duration, parkingSlot, essentials);
        } else {
            printf("Error: Invalid addParking command format\n");
        }
    }
    else if (strcmp(cmd, "addReservation") == 0) {
        if (sscanf(command, "%*s -%s %s %s %f %d %s %s", member, date, time, &duration, &parkingSlot, essentials[0], essentials[1]) >= 6) {
            addReservation(member, date, time, duration, parkingSlot, essentials);
        } else {
            printf("Error: Invalid addReservation command format\n");
        }
    }
    else if (strcmp(cmd, "addEvent") == 0) {
        if (sscanf(command, "%*s -%s %s %s %f %d %s %s", member, date, time, &duration, &parkingSlot, essentials[0], essentials[1]) >= 6) {
            addEvent(member, date, time, duration, parkingSlot, essentials);
        } else {
            printf("Error: Invalid addEvent command format\n");
        }
    }
    else if (strcmp(cmd, "bookEssentials") == 0) {
        if (sscanf(command, "%*s -%s %s %s %f %s", member, date, time, &duration, essentials[0]) >= 5) {
            bookEssentials(member, date, time, duration, essentials);
        } else {
            printf("Error: Invalid bookEssentials command format\n");
        }
    }
    else if (strcmp(cmd, "addBatch") == 0) {
        if (sscanf(command, "%*s -%s", filename) == 1) {
            addBatch(filename);
        } else {
            printf("Error: Invalid addBatch command format\n");
        }
    }
    else if (strcmp(cmd, "printBookings") == 0) {
        if (sscanf(command, "%*s -%s", algorithm) == 1) {
            processParallel(algorithm);
        } else {
            printf("Error: Invalid printBookings command format\n");
        }
    }
    else if (strcmp(cmd, "endProgram") == 0) {
        printf("-> Bye!\n");
        exit(0);
    }
    else {
        printf("Error: Invalid command\n");
    }
}

int main() {
    printf("~~ WELCOME TO PolyU ~~\n");
    char command[256];
    while (1) {
        printf("Please enter booking:\n");
        if (fgets(command, sizeof(command), stdin)) {
            command[strcspn(command, "\n")] = 0;
            processCommand(command);
        }
    }
    return 0;
}