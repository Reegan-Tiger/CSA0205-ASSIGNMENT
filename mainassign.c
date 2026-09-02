#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char part_name[50];
    int quantity;
    float cost;
    Date replacement_date;
    int recommended_interval_days;
    Date next_due_date;
} SparePart;

typedef struct {
    int service_id;
    Date service_date;
    char service_type[50];
    float mileage;
    float service_cost;
    char remarks[100];
    int part_count;
    SparePart parts[10];
} ServiceRecord;

typedef struct {
    char reg_number[20];
    char model[30];
    int year_manufacture;
    int service_count;
    ServiceRecord history[20];
} Vehicle;

typedef struct Customer {
    char customer_id[20];
    char name[50];
    char phone[15];
    Vehicle car;
    struct Customer *next;
} Customer;

Customer *head = NULL;

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int get_days_in_month(int month, int year) {
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) return 29;
    return days[month];
}

Date calculate_next_due_date(Date current, int interval_days) {
    Date result = current;
    result.day += interval_days;

    while (result.day > get_days_in_month(result.month, result.year)) {
        result.day -= get_days_in_month(result.month, result.year);
        result.month++;
        if (result.month > 12) {
            result.month = 1;
            result.year++;
        }
    }
    return result;
}

int date_to_days(Date d) {
    int total_days = d.year * 365 + d.day;
    for (int m = 1; m < d.month; m++) {
        total_days += get_days_in_month(m, d.year);
    }
    total_days += d.year / 4 - d.year / 100 + d.year / 400;
    return total_days;
}

int get_date_difference(Date d1, Date d2) {
    return date_to_days(d1) - date_to_days(d2);
}

void add_customer() {
    Customer *new_node = (Customer *)malloc(sizeof(Customer));
    if (!new_node) {
        printf("Memory allocation failed!\n");
        return;
    }
    new_node->next = NULL;
    new_node->car.service_count = 0;

    printf("\n--- Add Customer & Vehicle Details ---\n");
    printf("Customer ID: "); scanf("%s", new_node->customer_id);
    printf("Customer Name: "); scanf(" %[^\n]s", new_node->name);
    printf("Contact Number: "); scanf("%s", new_node->phone);
    printf("Car Reg Number: "); scanf("%s", new_node->car.reg_number);
    printf("Car Model: "); scanf(" %[^\n]s", new_node->car.model);
    printf("Year of Manufacture: "); scanf("%d", &new_node->car.year_manufacture);

    new_node->next = head;
    head = new_node;
    printf("Customer and vehicle added successfully!\n");
}

Customer* find_customer_by_reg(char *reg_num) {
    Customer *curr = head;
    while (curr != NULL) {
        if (strcmp(curr->car.reg_number, reg_num) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void record_service() {
    char reg_num[20];
    printf("\nEnter Vehicle Registration Number: ");
    scanf("%s", reg_num);

    Customer *cust = find_customer_by_reg(reg_num);
    if (!cust) {
        printf("Vehicle not registered in system!\n");
        return;
    }

    if (cust->car.service_count >= 20) {
        printf("Service log limit reached for this vehicle!\n");
        return;
    }

    ServiceRecord *sr = &cust->car.history[cust->car.service_count];
    sr->service_id = cust->car.service_count + 1;

    printf("Service Date (DD MM YYYY): ");
    scanf("%d %d %d", &sr->service_date.day, &sr->service_date.month, &sr->service_date.year);
    printf("Service Type: "); scanf(" %[^\n]s", sr->service_type);
    printf("Mileage (km): "); scanf("%f", &sr->mileage);
    printf("Service Cost: "); scanf("%f", &sr->service_cost);
    printf("Remarks: "); scanf(" %[^\n]s", sr->remarks);

    printf("Number of Spare Parts Replaced: ");
    scanf("%d", &sr->part_count);

    for (int i = 0; i < sr->part_count; i++) {
        printf("\n  Part %d Name: ", i + 1);
        scanf(" %[^\n]s", sr->parts[i].part_name);
        printf("  Quantity: "); scanf("%d", &sr->parts[i].quantity);
        printf("  Cost per unit: "); scanf("%f", &sr->parts[i].cost);
        printf("  Recommended Replacement Interval (in Days): ");
        scanf("%d", &sr->parts[i].recommended_interval_days);

        sr->parts[i].replacement_date = sr->service_date;
        sr->parts[i].next_due_date = calculate_next_due_date(sr->service_date, sr->parts[i].recommended_interval_days);

        printf("  -> Calculated Next Due Date: %02d/%02d/%04d\n",
               sr->parts[i].next_due_date.day,
               sr->parts[i].next_due_date.month,
               sr->parts[i].next_due_date.year);
    }

    cust->car.service_count++;
    printf("Service record updated successfully!\n");
}

void search_and_display_history() {
    char reg_num[20];
    printf("\nEnter Vehicle Registration Number: ");
    scanf("%s", reg_num);

    Customer *cust = find_customer_by_reg(reg_num);
    if (!cust) {
        printf("No records found for vehicle %s.\n", reg_num);
        return;
    }

    printf("\n========================================================\n");
    printf("CUSTOMER & VEHICLE DETAILS\n");
    printf("========================================================\n");
    printf("ID: %s | Name: %s | Phone: %s\n", cust->customer_id, cust->name, cust->phone);
    printf("Car: %s | Model: %s | Year: %d\n", cust->car.reg_number, cust->car.model, cust->car.year_manufacture);
    printf("Total Visits: %d\n", cust->car.service_count);

    for (int i = 0; i < cust->car.service_count; i++) {
        ServiceRecord sr = cust->car.history[i];
        printf("\n--------------------------------------------------------\n");
        printf("Service Visit #%d | Date: %02d/%02d/%04d | Type: %s\n",
               sr.service_id, sr.service_date.day, sr.service_date.month, sr.service_date.year, sr.service_type);
        printf("Mileage: %.1f km | Cost: $%.2f | Remarks: %s\n", sr.mileage, sr.service_cost, sr.remarks);

        if (sr.part_count > 0) {
            printf("Replaced Parts:\n");
            for (int j = 0; j < sr.part_count; j++) {
                printf("  - %s (Qty: %d, Cost: $%.2f) | Next Due: %02d/%02d/%04d\n",
                       sr.parts[j].part_name, sr.parts[j].quantity, sr.parts[j].cost,
                       sr.parts[j].next_due_date.day, sr.parts[j].next_due_date.month, sr.parts[j].next_due_date.year);
            }
        }
    }
}

void generate_reminders() {
    Date today;
    int threshold_days;

    printf("\nEnter Current Date (DD MM YYYY): ");
    scanf("%d %d %d", &today.day, &today.month, &today.year);
    printf("Enter Reminder Threshold (in days): ");
    scanf("%d", &threshold_days);

    printf("\n========================================================\n");
    printf("ADVANCE SPARE PART REPLACEMENT REMINDERS\n");
    printf("========================================================\n");

    int count = 0;
    Customer *curr = head;
    while (curr != NULL) {
        for (int i = 0; i < curr->car.service_count; i++) {
            ServiceRecord sr = curr->car.history[i];
            for (int j = 0; j < sr.part_count; j++) {
                int diff = get_date_difference(sr.parts[j].next_due_date, today);
                if (diff >= 0 && diff <= threshold_days) {
                    printf("REMINDER: Customer %s (%s) | Reg: %s\n", curr->name, curr->phone, curr->car.reg_number);
                    printf("  Part '%s' is due for replacement in %d day(s) [Due Date: %02d/%02d/%04d]\n\n",
                           sr.parts[j].part_name, diff,
                           sr.parts[j].next_due_date.day, sr.parts[j].next_due_date.month, sr.parts[j].next_due_date.year);
                    count++;
                }
            }
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("No spare parts due for replacement within %d days.\n", threshold_days);
    }
}

void analyze_service_data() {
    float total_expenses = 0.0;
    int total_services = 0;

    struct PartFreq {
        char name[50];
        int count;
    } frequencies[100];
    int unique_parts = 0;

    Customer *curr = head;
    while (curr != NULL) {
        total_services += curr->car.service_count;
        for (int i = 0; i < curr->car.service_count; i++) {
            ServiceRecord sr = curr->car.history[i];
            total_expenses += sr.service_cost;

            for (int j = 0; j < sr.part_count; j++) {
                total_expenses += (sr.parts[j].cost * sr.parts[j].quantity);
                int found = 0;
                for (int k = 0; k < unique_parts; k++) {
                    if (strcmp(frequencies[k].name, sr.parts[j].part_name) == 0) {
                        frequencies[k].count += sr.parts[j].quantity;
                        found = 1;
                        break;
                    }
                }
                if (!found && unique_parts < 100) {
                    strcpy(frequencies[unique_parts].name, sr.parts[j].part_name);
                    frequencies[unique_parts].count = sr.parts[j].quantity;
                    unique_parts++;
                }
            }
        }
        curr = curr->next;
    }

    printf("\n========================================================\n");
    printf("SERVICE CENTER ANALYTICAL REPORT\n");
    printf("========================================================\n");
    printf("Total Service Visits Processed: %d\n", total_services);
    printf("Total Revenue/Expenses Recorded: $%.2f\n", total_expenses);
    printf("\nSpare Part Replacement Frequencies:\n");

    for (int i = 0; i < unique_parts; i++) {
        printf("  - %-25s : Replaced %d time(s)\n", frequencies[i].name, frequencies[i].count);
    }
}

void save_to_file() {
    FILE *fp = fopen("service_center.dat", "wb");
    if (!fp) {
        printf("Error opening file for writing!\n");
        return;
    }

    Customer *curr = head;
    while (curr != NULL) {
        fwrite(curr->customer_id, sizeof(curr->customer_id), 1, fp);
        fwrite(curr->name, sizeof(curr->name), 1, fp);
        fwrite(curr->phone, sizeof(curr->phone), 1, fp);
        fwrite(&curr->car, sizeof(Vehicle), 1, fp);
        curr = curr->next;
    }
    fclose(fp);
    printf("All records saved to 'service_center.dat'.\n");
}

void load_from_file() {
    FILE *fp = fopen("service_center.dat", "rb");
    if (!fp) return;

    while (!feof(fp)) {
        Customer *new_node = (Customer *)malloc(sizeof(Customer));
        if (!new_node) break;

        if (fread(new_node->customer_id, sizeof(new_node->customer_id), 1, fp) != 1) {
            free(new_node);
            break;
        }
        fread(new_node->name, sizeof(new_node->name), 1, fp);
        fread(new_node->phone, sizeof(new_node->phone), 1, fp);
        fread(&new_node->car, sizeof(Vehicle), 1, fp);

        new_node->next = head;
        head = new_node;
    }
    fclose(fp);
}

int main() {
    load_from_file();
    int choice;

    do {
        printf("\n========================================================\n");
        printf(" CAR SERVICE & MAINTENANCE MANAGEMENT SYSTEM\n");
        printf("========================================================\n");
        printf("1. Add Customer & Vehicle\n");
        printf("2. Record Service Visit & Spare Parts\n");
        printf("3. Search & Display Vehicle Maintenance History\n");
        printf("4. Generate Advance Replacement Reminders\n");
        printf("5. Generate System Analytical Report\n");
        printf("6. Save Data & Exit\n");
        printf("Select Choice [1-6]: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_customer(); break;
            case 2: record_service(); break;
            case 3: search_and_display_history(); break;
            case 4: generate_reminders(); break;
            case 5: analyze_service_data(); break;
            case 6: save_to_file(); printf("Exiting system. Goodbye!\n"); break;
            default: printf("Invalid option. Try again!\n");
        }
    } while (choice != 6);

    return 0;
}