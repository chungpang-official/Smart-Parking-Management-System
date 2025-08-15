# 📦 SPMS_G60 Project

## 📂 Included Files
1. `Readme_G60.txt`
2. `SPMS_G60.c`
3. `SPMS_Report_G60.txt` — will be generated or truncated in the current directory
4. `Project_Report_G60.docx`
5. `test_data_G60.dat`
6. `demo_G60.mp4` — demonstrates how `test_data_G60.dat` is used to run the program

---

## 🛠 Compiling Process
```bash
gcc -std=c99 -o SPMS_G60 SPMS_G60.c
```
*(Uses the C99 standard on Linux Apollo)*

---

## 📋 Assumptions
- Five members: `member_A`, `member_B`, `member_C`, `member_D`, `member_E`
- Three **lockers**
- Three **umbrellas**
- Three **batteries**
- Three **cables**
- Three **valet parking**
- Three **inflation services**

**Test Period:**  
- From **10 May 2025** to **16 May 2025**  
- Priority booking slots: **08:00 AM – 20:00 PM**  
- One time slot = **1 hour**  
- Parking should have **12 time slots/day** for **7 days** to be filled in.

> **Note:** The project PDF on Blackboard states:  
> `"08:00 AM – 08:00 PM. One time slot is an hour of time. That means parking should have 24 time slots a day and there are 7 days to be filled in."`  
> This appears to be ambiguous; our implementation uses 12 slots/day in the priority window.

---

## 💻 Commands & Syntax

### Run the program
```bash
./SPMS
```
*(Simply enter `./SPMS` to start the program)*

### Add Parking
```bash
addParking –member_A 2025-05-16 10:00 3.0 battery
```

### Add Reservation
```bash
addReservation –member_B 2025-05-14 08:00 3.0 battery cable
```

### Add Event
```bash
addEvent –member_E 2025-05-16 14:00 2.0 locker umbrella valetpark
```

### Book Essentials
```bash
bookEssentials –member_C 2025-05-11 13:00 4.0 battery
```

### Add Batch
```bash
addBatch -xxxxx test_data_G60.dat
```

### Print Bookings
```bash
printBookings –xxx –[fcfs/prio/opti/ALL]
```

### End Program
```bash
endProgram
```
*(Ends the program, collects all child processes, and closes all files)*

---
