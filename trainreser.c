#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// File names for storing the train and booking records
#define TRAIN_FILE "trains.dat"
#define BOOKING_FILE "bookings.dat"

// Structure to store the details of the train
typedef struct {
    int train_no;
    char train_name[50];
    char from[30];
    char to[30];
    char departure[10];
    char arrival[10];
    int seats;
} Train;

// Structure to store booking details of the passenger
typedef struct {
    int pnr;
    int train_no;
    char name[50];
    int age;
    char gender;
    int seat_no;
} Booking;

// Function declarations
void mainMenu();
void adminPanel();
void userPanel();
void addTrain();
void viewTrains();
void bookTicket();
void viewBookings();
void cancelBooking();
int generatePNR();
int isTrainExist(int);
int isSeatAvailable(int);
int generateSeatNumber(int);

int main()      // Main Function
{
    mainMenu(); // Start with the main menu
}

// Display the main menu
void mainMenu() {
    int choice;
    while (1) {
        printf("\n--- Train Reservation System ---\n");
        printf("1. Admin Panel\n");
        printf("2. User Panel\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: adminPanel(); break;
            case 2: userPanel(); break;
            case 3: exit(0); // Exit program
            default: printf("Invalid choice.\n");
        }
    }
}

// Admin panel menu
void adminPanel() {
    int choice;
    while (1) {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add Train\n");
        printf("2. View All Trains\n");
        printf("3. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addTrain(); break;
            case 2: viewTrains(); break;
            case 3: return; // Back to main menu
            default: printf("Invalid choice.\n");
        }
    }
}

// User panel menu
void userPanel() {
    int choice;
    while (1) {
        printf("\n--- User Panel ---\n");
        printf("1. View Trains\n");
        printf("2. Book Ticket\n");
        printf("3. View Bookings\n");
        printf("4. Cancel Booking\n");
        printf("5. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewTrains(); break;
            case 2: bookTicket(); break;
            case 3: viewBookings(); break;
            case 4: cancelBooking(); break;
            case 5: return; // Back to main menu
            default: printf("Invalid choice.\n");
        }
    }
}

// Add a new train to the system
void addTrain() {
    FILE *fp = fopen(TRAIN_FILE, "ab"); // Append binary mode
    Train t;

    // Take input for train details
    printf("Enter Train Number: ");
    scanf("%d", &t.train_no);
    printf("Train Name: ");
    scanf(" %[^\n]", t.train_name);
    printf("From: ");
    scanf(" %[^\n]", t.from);
    printf("To: ");
    scanf(" %[^\n]", t.to);
    printf("Departure Time: ");
    scanf(" %[^\n]", t.departure);
    printf("Arrival Time: ");
    scanf(" %[^\n]", t.arrival);
    printf("Total Seats: ");
    scanf("%d", &t.seats);

    // Write train record to file
    fwrite(&t, sizeof(Train), 1, fp);
    fclose(fp);

    printf("Train added successfully.\n");
}

// View all available trains
void viewTrains() {
    FILE *fp = fopen(TRAIN_FILE, "rb");
    Train t;

    printf("\n%-10s %-20s %-10s %-10s %-10s %-10s %-5s\n", "Train No", "Name", "From", "To", "Dep", "Arr", "Seats");
    printf("-------------------------------------------------------------------------------\n");

    // Read and display each train record
    while (fread(&t, sizeof(Train), 1, fp)) {
        printf("%-10d %-20s %-10s %-10s %-10s %-10s %-5d\n", t.train_no, t.train_name, t.from, t.to, t.departure, t.arrival, t.seats);
    }

    fclose(fp);
}

// Book a ticket for a train
void bookTicket() {
    Booking b;
    FILE *fb = fopen(BOOKING_FILE, "ab"); // Open booking file in append mode
    int train_no;

    viewTrains(); // Show available trains
    printf("Enter Train Number to book: ");
    scanf("%d", &train_no);

    // Validate train existence and seat availability
    if (!isTrainExist(train_no)) {
        printf("Train does not exist.\n");
        fclose(fb);
        return;
    }

    if (!isSeatAvailable(train_no)) {
        printf("No seats available.\n");
        fclose(fb);
        return;
    }

    // Take passenger details
    b.pnr = generatePNR(); // Generate unique PNR
    b.train_no = train_no;
    printf("Enter Passenger Name: ");
    scanf(" %[^\n]", b.name);
    printf("Age: ");
    scanf("%d", &b.age);
    printf("Gender (M/F): ");
    scanf(" %c", &b.gender);
    b.seat_no = generateSeatNumber(train_no); // Assign seat

    // Save booking
    fwrite(&b, sizeof(Booking), 1, fb);
    fclose(fb);

    printf("Ticket booked successfully. PNR: %d, Seat No: %d\n", b.pnr, b.seat_no);
}

// View all bookings
void viewBookings() {
    FILE *fb = fopen(BOOKING_FILE, "rb");
    Booking b;

    printf("\n%-5s %-10s %-20s %-5s %-6s %-6s\n", "PNR", "Train No", "Name", "Age", "Gender", "Seat");
    printf("---------------------------------------------------------------\n");

    // Read and display all bookings
    while (fread(&b, sizeof(Booking), 1, fb)) {
        printf("%-5d %-10d %-20s %-5d %-6c %-6d\n", b.pnr, b.train_no, b.name, b.age, b.gender, b.seat_no);
    }

    fclose(fb);
}

// Cancel a booking using PNR
void cancelBooking() {
    int pnr;
    printf("Enter PNR to cancel: ");
    scanf("%d", &pnr);

    FILE *fb = fopen(BOOKING_FILE, "rb");
    FILE *ftemp = fopen("temp.dat", "wb"); // Temporary file for filtered records
    Booking b;
    int found = 0;

    // Copy all bookings except the one to be cancelled
    while (fread(&b, sizeof(Booking), 1, fb)) {
        if (b.pnr == pnr) {
            found = 1; // Skip this record
            continue;
        }
        fwrite(&b, sizeof(Booking), 1, ftemp); // Write valid records
    }

    fclose(fb);
    fclose(ftemp);

    // Replace original file with the temp file
    remove(BOOKING_FILE);
    rename("temp.dat", BOOKING_FILE);

    if (found)
        printf("Booking cancelled successfully.\n");
    else
        printf("PNR not found.\n");
}

// Generate a unique PNR and persist it in a file
int generatePNR() {
    FILE *fp;
    int pnr = 1000; // Default PNR

    // Try reading the last saved PNR
    fp = fopen("pnr_counter.dat", "rb");
    if (fp != NULL) {
        fread(&pnr, sizeof(int), 1, fp);
        fclose(fp);
    }

    int new_pnr = pnr + 1;

    // Save the new PNR for future use
    fp = fopen("pnr_counter.dat", "wb");
    if (fp != NULL) {
        fwrite(&new_pnr, sizeof(int), 1, fp);
        fclose(fp);
    } else {
        printf("Error updating PNR counter.\n");
    }

    return pnr;
}

// Check if a train with given train number exists
int isTrainExist(int train_no) {
    FILE *fp = fopen(TRAIN_FILE, "rb");
    Train t;

    while (fread(&t, sizeof(Train), 1, fp)) {
        if (t.train_no == train_no) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// Check if there are seats available in a train
int isSeatAvailable(int train_no) {
    FILE *fb = fopen(BOOKING_FILE, "rb");
    FILE *ft = fopen(TRAIN_FILE, "rb");
    Booking b;
    Train t;
    int booked = 0, total = 0;

    // Count number of booked seats
    while (fread(&b, sizeof(Booking), 1, fb)) {
        if (b.train_no == train_no)
            booked++;
    }

    // Get total seats from train data
    while (fread(&t, sizeof(Train), 1, ft)) {
        if (t.train_no == train_no) {
            total = t.seats;
            break;
        }
    }

    fclose(fb);
    fclose(ft);

    return booked < total; // True if seats are available
}

// Generate next seat number for a train
int generateSeatNumber(int train_no) {
    FILE *fb = fopen(BOOKING_FILE, "rb");
    Booking b;
    int seat = 1;

    // Count how many seats already booked to assign next seat
    while (fread(&b, sizeof(Booking), 1, fb)) {
        if (b.train_no == train_no)
            seat++;
    }

    fclose(fb);
    return seat;
}








