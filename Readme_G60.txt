Readme
Member: {23093488D Leung Chung Pang} {24018321D Wong Ki Sum} {23074098D Tso Chun Yeung} 
zipped file included:
  1.Readme_G60.txt
  2.SPMS_G60.c
  3.SPMS_Report_G60.txt // will be generated or truncated in the current directory
  4.Project_Report_G60.docx
  5.test_data_G60.dat
  6.demo_G60.mp4 // is to show you how we use the test_data_G60.dat to demonstrate the program.
Compiling process: 
gcc -std=c99 -o SPMS_G60 SPMS_G60.c // using C-standard on Linux apollo
Assumptions: 
➢ five members – member_A, member_B, member_C, member_D and member_E;  
➢ three lockers   
➢ three umbrellas   
➢ three batteries  
➢ three cables 
➢ three valet parking 
➢ three inflation services 
We test the booking schedule for a period, from 10 to 16 May, 2025. Priority will be given to the booking slots 
between 08:00 AM – 20:00 PM. One time slot is an hour of time. That means parking should have 12 time slots a 
day and there are 7 days to be filled in. // since the project pdf provided on blackboard is ambigous at this part. " 08:00 AM – 08:00 PM. One time slot is an hour of time. That means parking should have 24 time slots a 
day and there are 7 days to be filled in. "  quoted directly from blackboard.
Command & syntax:
./SPMS // Simply to enter [./SPMS] to start the program. 
addParking // addParking –member_A 2025-05-16 10:00 3.0 battery
addReservation // addReservation –member_B 2025-05-14 08:00 3.0  battery cable 
addEvent // addEvent –member_E 2025-05-16 14:00 2.0 locker umbrella valetpark
bookEssentials [bookEssentials –member_C 2025-05-11 13:00 4.0 battery;]
addBatch // addBatch -xxxxx [test_data_G60.dat]
printBookings // printBookings –xxx –[fcfs/prio/opti/ALL]
endProgram // This simply ends the program completely, upon collecting all the child processes and closing all the files.
