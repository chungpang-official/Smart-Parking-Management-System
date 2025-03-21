#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKINGS 100
#define MAX_ESSENTIALS 5

// Structure to represent a booking
typedef struct {
    char member[20];          // Member name
    char date[11];            // Date (YYYY-MM-DD)
    char time[6];             // Time (hh:mm)
    float duration;           // Duration in hours
    int parkingSlot;          // Parking slot number
    char essentials[MAX_ESSENTIALS][20]; // Essentials (e.g., battery, cable)
    char type[20];            // Type (e.g., Parking, Reservation, Event)
    int priority;             // Priority level
    int status;               // 1 = Accepted, 0 = Rejected
} Booking;

// Global variables
Booking bookings[MAX_BOOKINGS]; // Array to store bookings
int totalBookings = 0;          // Total number of bookings

// Function to add a booking
void addBooking(char member[], char date[], char time[], float duration, int parkingSlot, char essentials[][20], char type[], int priority, int status) {
    if (totalBookings >= MAX_BOOKINGS) {
        printf("Error: Maximum number of bookings reached.\n");
        return;
    }

    // Store the booking details
    strcpy(bookings[totalBookings].member, member);
    strcpy(bookings[totalBookings].date, date);
    strcpy(bookings[totalBookings].time, time);
    bookings[totalBookings].duration = duration;
    bookings[totalBookings].parkingSlot = parkingSlot;
    for (int i = 0; i < MAX_ESSENTIALS; i++) {
        strcpy(bookings[totalBookings].essentials[i], essentials[i]);
    }
    strcpy(bookings[totalBookings].type, type);
    bookings[totalBookings].priority = priority;
    bookings[totalBookings].status = status;

    totalBookings++;
    printf("-> [Pending]\n");
}

// Function to handle batch file input
void importBatch(char filename[]) {
    // Remove the leading '-' and trailing ';' from the filename
    if (filename[0] == '-') {
        memmove(filename, filename + 1, strlen(filename));
    }
    if (filename[strlen(filename) - 1] == ';') {
        filename[strlen(filename) - 1] = '\0'; // Remove the semicolon
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file '%s'. Please check if the file exists and has the correct permissions.\n", filename);
        return;
    }

    printf("Reading file: %s\n", filename);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("Line read: %s", line);

        // Parse the line and extract booking details
        char member[20], date[11], time[6], type[20], essentials[MAX_ESSENTIALS][20];
        float duration;
        int parkingSlot, priority, status;

        // Initialize essentials array
        for (int i = 0; i < MAX_ESSENTIALS; i++) {
            memset(essentials[i], 0, sizeof(essentials[i])); // Clear each essentials string
        }

        // Example parsing (adjust based on actual file format)
        int parsed = sscanf(line, "%s %s %s %f %d %s %s %s %d", member, date, time, &duration, &parkingSlot,
                            essentials[0], essentials[1], type, &status);

        if (parsed < 9) {
            printf("Error: Invalid format in file '%s'. Skipping line: %s", filename, line);
            continue;
        }

        // Determine priority based on type
        if (strcmp(type, "Event") == 0) priority = 3;
        else if (strcmp(type, "Reservation") == 0) priority = 2;
        else if (strcmp(type, "Parking") == 0) priority = 1;
        else priority = 0;

        addBooking(member, date, time, duration, parkingSlot, essentials, type, priority, status);
    }

    fclose(file);
    printf("-> [Pending]\n");
}

// Function to print accepted bookings
void printAcceptedBookings(char algorithm[]) {
    printf("*** Parking Booking – ACCEPTED / %s ***\n", algorithm);

    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 1) { // Only print accepted bookings
            printf("\n%s has the following bookings:\n", bookings[i].member);
            printf("| %-10s | %-5s | %-5s | %-11s | %-15s |\n", "Date", "Start", "End", "Type", "Device");
            printf("|------------|-------|-------|-------------|-----------------|\n");

            // Calculate end time
            int startHour, startMinute;
            sscanf(bookings[i].time, "%d:%d", &startHour, &startMinute);
            int endHour = startHour + (int)bookings[i].duration;
            int endMinute = startMinute + (int)((bookings[i].duration - (int)bookings[i].duration) * 60);
            if (endMinute >= 60) {
                endHour++;
                endMinute -= 60;
            }
            char endTime[6];
            sprintf(endTime, "%02d:%02d", endHour, endMinute);

            // Print booking details
            printf("| %-10s | %-5s | %-5s | %-11s | ", bookings[i].date, bookings[i].time, endTime, bookings[i].type);

            // Print essentials
            char deviceColumn[20] = ""; // Buffer to store the Device column content
            for (int j = 0; j < MAX_ESSENTIALS; j++) {
                if (strlen(bookings[i].essentials[j]) > 0) {
                    strcat(deviceColumn, bookings[i].essentials[j]);
                    strcat(deviceColumn, " ");
                }
            }

            // Print the Device column with fixed width
            printf("%-15s |\n", deviceColumn);
        }
    }
    printf("\n– End –\n");
}

// Function to print rejected bookings
void printRejectedBookings(char algorithm[]) {
    printf("\n*** Parking Booking – REJECTED / %s ***\n", algorithm);

    int rejectedCount = 0;
    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) { // Only print rejected bookings
            rejectedCount++;
        }
    }

    for (int i = 0; i < totalBookings; i++) {
        if (bookings[i].status == 0) {
            printf("\n%s (there are %d bookings rejected):\n", bookings[i].member, rejectedCount);
            printf("| %-10s | %-5s | %-5s | %-11s | %-15s |\n", "Date", "Start", "End", "Type", "Essentials");
            printf("|------------|-------|-------|-------------|-----------------|\n");

            // Calculate end time
            int startHour, startMinute;
            sscanf(bookings[i].time, "%d:%d", &startHour, &startMinute);
            int endHour = startHour + (int)bookings[i].duration;
            int endMinute = startMinute + (int)((bookings[i].duration - (int)bookings[i].duration) * 60);
            if (endMinute >= 60) {
                endHour++;
                endMinute -= 60;
            }
            char endTime[6];
            sprintf(endTime, "%02d:%02d", endHour, endMinute);

            // Print booking details
            printf("| %-10s | %-5s | %-5s | %-11s | ", bookings[i].date, bookings[i].time, endTime, bookings[i].type);

            // Print essentials
            char essentialsColumn[20] = ""; // Buffer to store the Essentials column content
            for (int j = 0; j < MAX_ESSENTIALS; j++) {
                if (strlen(bookings[i].essentials[j]) > 0) {
                    strcat(essentialsColumn, bookings[i].essentials[j]);
                    strcat(essentialsColumn, " ");
                }
            }

            // Print the Essentials column with fixed width
            printf("%-15s |\n", essentialsColumn);
        }
    }
    printf("\n– End –\n");
}

// Function to print all bookings
void printBookings(char algorithm[]) {
    printAcceptedBookings(algorithm);
    printRejectedBookings(algorithm);
    printf("-> [Done!]\n");
}

// Function to handle user commands
void processCommand(char command[]) {
    char cmd[20], member[20], date[11], time[6], essentials[MAX_ESSENTIALS][20], type[20], filename[20], algorithm[10];
    float duration;
    int parkingSlot;

    if (sscanf(command, "%s", cmd) == 1) {
        if (strcmp(cmd, "addParking") == 0) {
            sscanf(command, "%*s -%s %s %s %f %d %s %s", member, date, time, &duration, &parkingSlot, essentials[0], essentials[1]);
            addBooking(member, date, time, duration, parkingSlot, essentials, "Parking", 1, 1);
        } else if (strcmp(cmd, "addReservation") == 0) {
            sscanf(command, "%*s -%s %s %s %f %d %s %s", member, date, time, &duration, &parkingSlot, essentials[0], essentials[1]);
            addBooking(member, date, time, duration, parkingSlot, essentials, "Reservation", 2, 1);
        } else if (strcmp(cmd, "bookEssentials") == 0) {
            sscanf(command, "%*s -%s %s %s %f %s", member, date, time, &duration, essentials[0]);
            addBooking(member, date, time, duration, 0, essentials, "Essentials", 0, 1);
        } else if (strcmp(cmd, "importBatch") == 0) {
            sscanf(command, "%*s -%s", filename);
            importBatch(filename);
        } else if (strcmp(cmd, "printBookings") == 0) {
            sscanf(command, "%*s -%s", algorithm);
            printBookings(algorithm);
        } else if (strcmp(cmd, "endProgram") == 0) {
            printf("-> Bye!\n");
            exit(0);
        } else {
            printf("Error: Invalid command.\n");
        }
    }
}

// Main function
int main() {
    char command[256];

    printf("~* WELCOME TO PolyU ~*\n");

    while (1) {
        printf("Please enter booking:\n");
        fgets(command, sizeof(command), stdin);

        // Remove newline character from input
        command[strcspn(command, "\n")] = 0;

        // Process the command
        processCommand(command);
    }

    return 0;
}