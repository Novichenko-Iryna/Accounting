#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>

#define MAX_LEN 100
#define MAX_ORDERS 100
#define MAX_PARTNERS 100
#define MAX_SERVICES 100
#define MAX_CONTRACTS 100
#define MAX_INVOICES 1000
#define MAX_PAYMENTS 100

typedef struct {
    int id;
    wchar_t role[20]; //Customer (Замовник) чи Performer (Виконавець)
    wchar_t name[MAX_LEN]; // назва Партнера
    wchar_t first_name[MAX_LEN]; // прізвище керівника
    wchar_t last_name[MAX_LEN]; // ім'я керівника
    wchar_t fathers_name[MAX_LEN]; //по батькові керівника
    wchar_t position[MAX_LEN]; // посада 
    wchar_t address[MAX_LEN]; //юридична адреса
    wchar_t tax_code[20]; // код за ЄДРПОУ
    wchar_t start_date[20]; //дата початку контракту
    wchar_t contract_number[20]; // номер договору
    wchar_t details[MAX_LEN]; //назва договору
} Partner;

typedef struct {
    int id;
    wchar_t appointment[20];         // "Received" або "Provided"
    wchar_t service_name[MAX_LEN];   // Назва послуги
    wchar_t balance_account[20];     // бухгалтерськи рахунок
} Service;

typedef struct {
    int id;
    int customer_id; // ID замоника
    int performer_id; // ID перевізника
    int service_id; // ID послуги
    wchar_t date_of_order[20]; // дата замовлення
    wchar_t loading_place[100]; // місце завантаження
    wchar_t unloading_place[100]; //місце розвантаження
    double freight; // Вартість для замовника
    double margin; // Ціна від перевізника
    wchar_t vehicle_number[20]; // номер автомобіля
    wchar_t date_of_unloading[20]; // дата розвантаження
    int invoice_issued; // 0 — не створено, 1 — рахунок створено
} Order;

typedef struct {
    int invoice_id;
    wchar_t invoice_type[20]; // рахунок замовнику чи рахунок від виконавця
    wchar_t invoice_number[20]; // номер рахунку
    int order_id; // ID замовлення
    int customer_id; // ID замовника
    int service_id; // ID послуги
    wchar_t service_name[MAX_LEN]; // назва послуги
    double amount_due; // сума до сплати
    wchar_t invoice_date[20]; // дата рахунку
    } Invoice;

typedef struct {
    int id; // Унікальний номер акту
    wchar_t date[20]; // Дата акту, вводиться вручну
    wchar_t invoice_number[20]; // Номер інвойса
    int invoice_id; // ID інвойса (або посилання через order_id)
} Act;

typedef struct {
    int payment_id;  // Унікальний номер платежу
    wchar_t payment_type[20]; // "від замовника" або "виконавцю"
    int invoice_id; // ID рахунку
    int partner_id; // ID партнера
    double amount_paid; // оплачена сума
    wchar_t payment_date[20]; // формат РРРР-ММ-ДД
} Payment;


void trim_newline(wchar_t* str) {
    size_t len = wcslen(str);
    if (len > 0 && str[len - 1] == L'\n') {
        str[len - 1] = L'\0';
    }
}

Partner* find_partner_by_id(Partner* partners, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (partners[i].id == id) return &partners[i];
    }
    return NULL;
}

Service* find_service_by_id(Service* services, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (services[i].id == id) return &services[i];
    }
    return NULL;
}

Partner partners[MAX_PARTNERS];
int partner_count;

Service services[MAX_SERVICES];
int service_count;

Order orders[MAX_ORDERS];
int order_count;

void add_partner() {
    Partner partner;
    FILE* file = _wfopen(L"partners.txt", L"a+, ccs=UNICODE");

    if (!file) {
        wprintf(L"Не вдалося відкрити файл для запису.\n");
        return;
    }

    wprintf(L"\n=== Додавання партнера ===\n");

    wprintf(L"ID партнера: ");
    wchar_t input[10];
    fgetws(input, 10, stdin);
    swscanf(input, L"%d", &partner.id);

    wprintf(L"Роль (Customer/Performer): ");
    fgetws(partner.role, 20, stdin); trim_newline(partner.role);

    wprintf(L"Назва компанії: ");
    fgetws(partner.name, MAX_LEN, stdin); trim_newline(partner.name);

    wprintf(L"Прізвище керівника: ");
    fgetws(partner.last_name, MAX_LEN, stdin); trim_newline(partner.last_name);

    wprintf(L"Ім'я керівника: ");
    fgetws(partner.first_name, MAX_LEN, stdin); trim_newline(partner.first_name);

    wprintf(L"По батькові керівника: ");
    fgetws(partner.fathers_name, MAX_LEN, stdin); trim_newline(partner.fathers_name);

    wprintf(L"Посада керівника: ");
    fgetws(partner.position, MAX_LEN, stdin); trim_newline(partner.position);

    wprintf(L"Адреса: ");
    fgetws(partner.address, MAX_LEN, stdin); trim_newline(partner.address);

    wprintf(L"ІПН (податковий код): ");
    fgetws(partner.tax_code, 20, stdin); trim_newline(partner.tax_code);

    wprintf(L"Дата початку контракту (YYYY-MM-DD): ");
    fgetws(partner.start_date, 20, stdin); trim_newline(partner.start_date);

    wprintf(L"Номер контракту: ");
    fgetws(partner.contract_number, 20, stdin); trim_newline(partner.contract_number);

    wprintf(L"Опис (наприклад: Транспортні послуги): ");
    fgetws(partner.details, MAX_LEN, stdin); trim_newline(partner.details);

    fwprintf(file, L"%d;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls\n",
        partner.id,
        partner.role,
        partner.name,
        partner.last_name,
        partner.first_name,
        partner.fathers_name,
        partner.position,
        partner.address,
        partner.tax_code,
        partner.start_date,
        partner.contract_number,
        partner.details);

    fclose(file);
    wprintf(L"Партнера додано успішно!\n");
}

void list_partners() {
    Partner partner;
    FILE* file = _wfopen(L"partners.txt", L"r, ccs=UNICODE");

    if (!file) {
        wprintf(L"Не вдалося відкрити файл для читання.\n");
        return;
    }

    wprintf(L"\n=== Список партнерів ===\n");

    while (fwscanf(file, L"%d;%19[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%19[^;];%19[^;];%19[^;];%99[^\n]\n",
        &partner.id,
        partner.role,
        partner.name,
        partner.last_name,
        partner.first_name,
        partner.fathers_name,
        partner.position,
        partner.address,
        partner.tax_code,
        partner.start_date,
        partner.contract_number,
        partner.details) == 12) {

        wprintf(L"\nID: %d\n", partner.id);
        wprintf(L"Роль: %ls\n", partner.role);
        wprintf(L"Назва: %ls\n", partner.name);
        wprintf(L"Керівник: %ls %ls %ls (%ls)\n",
            partner.last_name, partner.first_name, partner.fathers_name, partner.position);
        wprintf(L"Адреса: %ls\n", partner.address);
        wprintf(L"ІПН: %ls\n", partner.tax_code);
        wprintf(L"Дата початку контракту: %ls\n", partner.start_date);
        wprintf(L"Номер контракту: %ls\n", partner.contract_number);
        wprintf(L"Деталі: %ls\n", partner.details);
    }

    fclose(file);
}

void delete_partner_by_id(int target_id) {
    FILE* file = _wfopen(L"partners.txt", L"r, ccs=UNICODE");
    FILE* temp = _wfopen(L"temp.txt", L"w, ccs=UNICODE");

    if (!file || !temp) {
        wprintf(L"Не вдалося відкрити файли для обробки.\n");
        if (file) fclose(file);
        if (temp) fclose(temp);
        return;
    }

    Partner partner;
    int found = 0;

    while (fwscanf(file, L"%d;%19[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%19[^;];%19[^;];%19[^;];%99[^\n]\n",
        &partner.id,
        partner.role,
        partner.name,
        partner.last_name,
        partner.first_name,
        partner.fathers_name,
        partner.position,
        partner.address,
        partner.tax_code,
        partner.start_date,
        partner.contract_number,
        partner.details) == 12) {

        if (partner.id != target_id) {
            fwprintf(temp, L"%d;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls\n",
                partner.id,
                partner.role,
                partner.name,
                partner.last_name,
                partner.first_name,
                partner.fathers_name,
                partner.position,
                partner.address,
                partner.tax_code,
                partner.start_date,
                partner.contract_number,
                partner.details);
        }
        else {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    if (found) {
        _wremove(L"partners.txt");
        _wrename(L"temp.txt", L"partners.txt");
        wprintf(L"Партнера з ID %d успішно видалено.\n", target_id);
    }
    else {
        _wremove(L"temp.txt");
        wprintf(L"Партнера з ID %d не знайдено.\n", target_id);
    }
}

void delete_partner() {
    int id;
    wprintf(L"Введіть ID партнера, якого потрібно видалити: ");
    wchar_t input[10];
    fgetws(input, 10, stdin);
    swscanf(input, L"%d", &id);
    delete_partner_by_id(id);
}

void add_service() {
    FILE* file = _wfopen(L"services.dat", L"ab");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл для запису.\n");
        return;
    }

    Service s;
    wprintf(L"\n--- Додавання послуги ---\n");
    wprintf(L"ID послуги: ");
    wscanf(L"%d", &s.id);
    while (getwchar() != L'\n'); // очищає ввід до кінця рядка

    wprintf(L"Тип послуги (Received/Provided): ");
    fgetws(s.appointment, 20, stdin);
    s.appointment[wcslen(s.appointment) - 1] = L'\0';

    wprintf(L"Назва послуги: ");
    fgetws(s.service_name, MAX_LEN, stdin);
    s.service_name[wcslen(s.service_name) - 1] = L'\0';

    wprintf(L"Код рахунку (напр. 631): ");
    fgetws(s.balance_account, 20, stdin);
    s.balance_account[wcslen(s.balance_account) - 1] = L'\0';

    fwrite(&s, sizeof(Service), 1, file);
    fclose(file);

    wprintf(L"Послугу додано успішно!\n");
}

void list_services() {
    FILE* file = _wfopen(L"services.dat", L"rb");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл для читання.\n");
        return;
    }

    Service s;
    wprintf(L"\n--- Список послуг ---\n");

    while (fread(&s, sizeof(Service), 1, file) == 1) {
        wprintf(L"ID: %d\n", s.id);
        wprintf(L"Тип: %ls\n", s.appointment);
        wprintf(L"Назва: %ls\n", s.service_name);
        wprintf(L"Рахунок: %ls\n", s.balance_account);
        wprintf(L"------------------------\n");
    }

    fclose(file);
}

void load_partners() {
    FILE* file = _wfopen(L"partners.txt", L"r, ccs=UNICODE");
    if (!file) {
        wprintf(L"Не вдалося відкрити partners.txt для читання.\n");
        return;
    }

    partner_count = 0;
    while (fwscanf(file, L"%d;%19[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%19[^;];%19[^;];%19[^;];%99[^\n]\n",
        &partners[partner_count].id,
        partners[partner_count].role,
        partners[partner_count].name,
        partners[partner_count].last_name,
        partners[partner_count].first_name,
        partners[partner_count].fathers_name,
        partners[partner_count].position,
        partners[partner_count].address,
        partners[partner_count].tax_code,
        partners[partner_count].start_date,
        partners[partner_count].contract_number,
        partners[partner_count].details) == 12) {
        partner_count++;
        if (partner_count >= MAX_PARTNERS) break;
    }

    fclose(file);
}

void load_services() {
    FILE* file = _wfopen(L"services.dat", L"rb");
    if (!file) {
        wprintf(L"Не вдалося відкрити services.dat для читання.\n");
        return;
    }

    service_count = 0;
    while (fread(&services[service_count], sizeof(Service), 1, file) == 1) {
        service_count++;
        if (service_count >= MAX_SERVICES) break;
    }

    fclose(file);
}

void add_order() {
    FILE* file = _wfopen(L"orders.dat", L"ab");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл для запису.\n");
        return;
    }

    Order o;
    wprintf(L"\n--- Додавання замовлення ---\n");

    wprintf(L"ID замовлення: ");
    wscanf(L"%d", &o.id); while (getwchar() != L'\n');

    wprintf(L"ID замовника: ");
    wscanf(L"%d", &o.customer_id); while (getwchar() != L'\n');

    wprintf(L"ID виконавця: ");
    wscanf(L"%d", &o.performer_id); while (getwchar() != L'\n');

    wprintf(L"ID послуги: ");
    wscanf(L"%d", &o.service_id); while (getwchar() != L'\n');

    wprintf(L"Дата замовлення (YYYY-MM-DD): ");
    fgetws(o.date_of_order, 20, stdin); o.date_of_order[wcslen(o.date_of_order) - 1] = L'\0';

    wprintf(L"Місце завантаження: ");
    fgetws(o.loading_place, 100, stdin); o.loading_place[wcslen(o.loading_place) - 1] = L'\0';

    wprintf(L"Місце розвантаження: ");
    fgetws(o.unloading_place, 100, stdin); o.unloading_place[wcslen(o.unloading_place) - 1] = L'\0';

    wprintf(L"Вартсть замовнику: ");
    wscanf(L"%lf", &o.freight); while (getwchar() != L'\n');

    wprintf(L"Вартість від перевізника: ");
    wscanf(L"%lf", &o.margin); while (getwchar() != L'\n');

    wprintf(L"Номер а/м: ");
    fgetws(o.vehicle_number, 20, stdin); o.vehicle_number[wcslen(o.vehicle_number) - 1] = L'\0';

    wprintf(L"Дата розвантаження (YYYY-MM-DD): ");
    fgetws(o.date_of_unloading, 20, stdin); o.date_of_unloading[wcslen(o.date_of_unloading) - 1] = L'\0';

    o.invoice_issued = 0;

    fwrite(&o, sizeof(Order), 1, file);
    fclose(file);

    wprintf(L"Замовлення додано успішно!\n");
}

void list_orders(Order* orders, int order_count, Partner* partners, int partner_count, Service* services, int service_count) {
    wprintf(L"\n=== Список замовлень ===\n");
    for (int i = 0; i < order_count; i++) {
        Partner* customer = find_partner_by_id(partners, partner_count, orders[i].customer_id);
        Partner* performer = find_partner_by_id(partners, partner_count, orders[i].performer_id);
        Service* service = find_service_by_id(services, service_count, orders[i].service_id);

        wprintf(L"\nЗамовлення ID: %d\n", orders[i].id);

        if (customer) {
            wprintf(L"Замовник: %ls\n", customer->name);
        }
        else {
            wprintf(L"Замовник: [ID %d не знайдено]\n", orders[i].customer_id);
        }

        if (performer) {
            wprintf(L"Виконавець: %ls\n", performer->name);
        }
        else {
            wprintf(L"Виконавець: [ID %d не знайдено]\n", orders[i].performer_id);
        }

        if (service) {
            wprintf(L"Послуга: %ls\n", service->service_name, service->appointment);
        }
        else {
            wprintf(L"Послуга: [ID %d не знайдено]\n", orders[i].service_id);
        }

        wprintf(L"Дата замовлення: %ls\n", orders[i].date_of_order);
        wprintf(L"Місце завантаження: %ls\n", orders[i].loading_place);
        wprintf(L"Місце розвантаження: %ls\n", orders[i].unloading_place);
        wprintf(L"Ціна для клієнта: %.2lf\n", orders[i].freight);
        wprintf(L"Ціна від перевізника: %.2lf\n", orders[i].margin);
        wprintf(L"А/м номер: %ls\n", orders[i].vehicle_number);
        wprintf(L"Дата розвантаження: %ls\n", orders[i].date_of_unloading);
        //wprintf(L"ID рахунку: %d\n", orders[i].invoice_id);
        wprintf(L"------------------------\n");
    }
}

int load_orders(Order* orders) {
    FILE* file = _wfopen(L"orders.dat", L"rb");
    if (!file) {
        wprintf(L"Не вдалося відкрити orders.dat для читання.\n");
        return 0;
    }

    int count = 0;
    while (fread(&orders[count], sizeof(Order), 1, file) == 1 && count < MAX_ORDERS) {
        count++;
    }

    fclose(file);
    return count;
}

void delete_order_by_id(int delete_id) {
    FILE* file = _wfopen(L"orders.dat", L"rb");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл orders.dat для читання.\n");
        return;
    }

    Order temp_orders[MAX_ORDERS];
    int temp_count = 0;
    int found = 0;

    // Зчитування замовлень
    while (fread(&temp_orders[temp_count], sizeof(Order), 1, file) == 1) {
        if (temp_orders[temp_count].id == delete_id) {
            found = 1; // Знайдено замовлення для видалення
        }
        else {
            temp_count++;
        }
    }
    fclose(file);

    if (!found) {
        wprintf(L"Замовлення з ID %d не знайдено.\n", delete_id);
        return;
    }

    // Перезапис файлу без видаленого замовлення
    file = _wfopen(L"orders.dat", L"wb");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл для запису.\n");
        return;
    }

    for (int i = 0; i < temp_count; i++) {
        fwrite(&temp_orders[i], sizeof(Order), 1, file);
    }
    fclose(file);

    wprintf(L"Замовлення з ID %d успішно видалено.\n", delete_id);
}

void edit_order() {
    order_count = load_orders(orders);
    load_partners();
    load_services();

    FILE* file = _wfopen(L"orders.dat", L"r+b");
    if (!file) {
        wprintf(L"Не вдалося відкрити orders.dat для читання.\n");
        return;
    }

    int id;
    wprintf(L"\nВведіть ID замовлення для редагування: ");
    wscanf(L"%d", &id); while (getwchar() != L'\n');

    int index = -1;
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        wprintf(L"Замовлення з таким ID не знайдено.\n");
        return;
    }

    Order* o = &orders[index];
    wprintf(L"\n--- Редагування замовлення ID %d ---\n", o->id);

    wprintf(L"Нове місце завантаження (поточне: %ls): ", o->loading_place);
    fgetws(o->loading_place, 100, stdin);
    o->loading_place[wcslen(o->loading_place) - 1] = L'\0';

    wprintf(L"Нове місце розвантаження (поточне: %ls): ", o->unloading_place);
    fgetws(o->unloading_place, 100, stdin);
    o->unloading_place[wcslen(o->unloading_place) - 1] = L'\0';

    wprintf(L"Нова вартість замовнику (поточна: %.2lf): ", o->freight);
    wscanf(L"%lf", &o->freight); while (getwchar() != L'\n');

    wprintf(L"Нова вартість від перевізника (поточна: %.2lf): ", o->margin);
    wscanf(L"%lf", &o->margin); while (getwchar() != L'\n');

    wprintf(L"Новий номер а/м (поточний: %ls): ", o->vehicle_number);
    fgetws(o->vehicle_number, 20, stdin);
    o->vehicle_number[wcslen(o->vehicle_number) - 1] = L'\0';

    wprintf(L"Нова дата розвантаження (YYYY-MM-DD, поточна: %ls): ", o->date_of_unloading);
    fgetws(o->date_of_unloading, 20, stdin);
    o->date_of_unloading[wcslen(o->date_of_unloading) - 1] = L'\0';

    // Клієнт
    wprintf(L"\n--- Список клієнтів ---\n");
    for (int i = 0; i < partner_count; i++) {
        wprintf(L"ID: %d, Назва: %ls\n", partners[i].id, partners[i].name);
    }
    wprintf(L"Новий ID клієнта (поточний: %d): ", o->customer_id);
    wscanf(L"%d", &o->customer_id); while (getwchar() != L'\n');

    // Виконавець
    wprintf(L"\n--- Список виконавців ---\n");
    for (int i = 0; i < partner_count; i++) {
        wprintf(L"ID: %d, Назва: %ls\n", partners[i].id, partners[i].name);
    }
    wprintf(L"Новий ID виконавця (поточний: %d): ", o->performer_id);
    wscanf(L"%d", &o->performer_id); while (getwchar() != L'\n');

    // Послуга
    wprintf(L"\n--- Список послуг ---\n");
    for (int i = 0; i < service_count; i++) {
        wprintf(L"ID: %d, Назва: %ls\n", services[i].id, services[i].service_name);
    }
    wprintf(L"Новий ID послуги (поточний: %d): ", o->service_id);
    wscanf(L"%d", &o->service_id); while (getwchar() != L'\n');

    fseek(file, index * sizeof(Order), SEEK_SET); // ← позиціонування
    fwrite(&orders[index], sizeof(Order), 1, file);
    fclose(file);

    wprintf(L"\nЗамовлення успішно оновлено.\n");
}

void generate_invoice_number(wchar_t* buffer, int max) {
    static int counter = 1; // в реальності краще зберігати це в окремому файлі
    swprintf(buffer, max, L"INV%05d", counter++);
}

int get_next_invoice_id() {
    FILE* file = _wfopen(L"last_invoice_id.txt", L"r, ccs=UNICODE");
    int last_id = 0;
    if (file) {
        fwscanf(file, L"%d", &last_id);
        fclose(file);
    }

    int next_id = last_id + 1;

    file = _wfopen(L"last_invoice_id.txt", L"w, ccs=UNICODE");
    if (file) {
        fwprintf(file, L"%d", next_id);
        fclose(file);
    }

    return next_id;
}

void create_invoice() {

    _wmkdir(L"Invoices");

    Order orders[MAX_ORDERS];
    int order_count = load_orders(orders);
    load_partners();
    load_services();

    int order_id;
    wprintf(L"Введіть ID замовлення, за яким створити рахунок: ");
    wscanf(L"%d", &order_id); while (getwchar() != L'\n');

    Order* o = NULL;
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == order_id) {
            o = &orders[i];
            break;
        }
    }

    if (!o) {
        wprintf(L"Замовлення не знайдено.\n");
        return;
    }

    Partner* customer = NULL;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id == o->customer_id) {
            customer = &partners[i];
            break;
        }
    }

    
    Service* service = NULL;
    for (int i = 0; i < service_count; i++) {
        if (services[i].id == o->service_id) {
            service = &services[i];
            break;
        }
    }

    if (!customer || !service) {
        wprintf(L"Не знайдено клієнта або послугу.\n");
        return;
    
        Order orders[MAX_ORDERS];
        int order_count = load_orders(orders);

        for (int i = 0; i < order_count; i++) {
            if (orders[i].id == order_id) {
                orders[i].invoice_issued = 1;
                break;
            }
        }

        FILE* file = _wfopen(L"orders.dat", L"wb");
        if (!file) {
            wprintf(L"Не вдалося відкрити файл для оновлення рахунка.\n");
            return;
        }



        fwrite(orders, sizeof(Order), order_count, file);
        fclose(file);

    }

    Invoice inv;
    inv.invoice_id = get_next_invoice_id();  // Спочатку генеруємо числовий ID
    generate_invoice_number(inv.invoice_number, 20);  // Потім — текстовий номер
    wcscpy(inv.invoice_type, L"Customer_Invoice");
    inv.order_id = o->id;
    inv.customer_id = customer->id;
    inv.service_id = service->id;
    wcscpy(inv.service_name, service->service_name);
    inv.amount_due = o->freight;

    wprintf(L"Введіть дату рахунку (YYYY-MM-DD): ");
    fgetws(inv.invoice_date, 20, stdin);
    inv.invoice_date[wcslen(inv.invoice_date) - 1] = L'\0';

    // Формування шляху до файлу
    wchar_t filename[100];
    swprintf(filename, 100, L"Invoices\\%ls.txt", inv.invoice_number);

    FILE* f = _wfopen(filename, L"w, ccs=UTF-8");
    if (!f) {
        wprintf(L"Не вдалося створити файл рахунку.\n");
        return;
    }

    fwprintf(f, L"ID рахунку: %d\n", inv.invoice_id);
    fwprintf(f, L"Рахунок на оплату № %ls від %ls\n\n\n", inv.invoice_number, inv.invoice_date);
    fwprintf(f, L"П/р UA343052990000026009006804903, Банк АТ КБ \"Приватбанк\", МФО 305299\n");
    fwprintf(f, L"08303, Київська обл., м. Бориспіль, вул. Нова, буд. 4, кв.72, тел.: 0673747000\n");
    fwprintf(f, L"код за ЄДРПОУ 37243792\nПлатник єдиного податку 3 група 5%%\n\n\n");
    fwprintf(f, L"Постачальник:\t\t\tТОВ \"ІНТАСК\"\n\n");
    fwprintf(f, L"Покупець:\t\t\t%ls\n", customer->name);

    fwprintf(f, L"Згідно договору: %ls, №%ls від %ls\n\n",
        customer->details, customer->contract_number, customer->start_date);

   

    fwprintf(f, L"За %ls з %ls до %ls, а/м %ls\n",
        service->service_name, o->loading_place, o->unloading_place, o->vehicle_number);

    fwprintf(f, L"\nВсього найменувань 1, на суму %.2lf грн.\n\n\n\n\n", inv.amount_due);
    fwprintf(f, L"Рахунок виписав:\t\t\t Директор Даценко С.М.\n");
    fclose(f);

    // Зберігаємо в файл .dat
    FILE* df = _wfopen(L"invoices.dat", L"ab");
    if (!df) {
        wprintf(L"Не вдалося відкрити файл invoices.dat для запису.\n");
        return;
    }
    fwrite(&inv, sizeof(Invoice), 1, df);
    fclose(df);


    wprintf(L"\nРахунок успішно сформовано: %ls\n", filename);
}

void list_invoices() {
    struct _wfinddata_t file;
    intptr_t handle;
    int count = 0;

    handle = _wfindfirst(L"Invoices\\*.txt", &file);
    if (handle == -1) {
        wprintf(L"Рахунків не знайдено або папка Invoices не існує.\n");
        return;
    }

    wprintf(L"\n=== Список рахунків ===\n");

    do {
        wchar_t filepath[260];
        swprintf(filepath, 260, L"Invoices\\%ls", file.name);

        FILE* f = _wfopen(filepath, L"r, ccs=UNICODE");
        if (f) {
            wchar_t line[512];
            int invoice_id = -1;

            // Зчитуємо перший рядок, де очікується invoice_id
            if (fgetws(line, 512, f)) {
                // Спроба витягнути invoice_id з першого рядка
                if (swscanf(line, L"ID рахунку: %d", &invoice_id) == 1) {
                    wprintf(L"\n--- [%d] %ls ---\n", invoice_id, file.name);
                    wprintf(L"%ls", line); // Виводимо перший рядок
                }
                else {
                    wprintf(L"\n--- [?] %ls ---\n", file.name);
                    wprintf(L"%ls", line); // все одно виводимо
                }
            }

            // Виводимо решту рядків
            while (fgetws(line, 512, f)) {
                wprintf(L"%ls", line);
            }

            fclose(f);
        }
        else {
            wprintf(L"Не вдалося відкрити файл %ls\n", file.name);
        }

        count++;
    } while (_wfindnext(handle, &file) == 0);

    _findclose(handle);

    if (count == 0) {
        wprintf(L"Жодного рахунку не знайдено.\n");
    }
}

int load_invoices(Invoice invoices[]) {
    FILE* file = _wfopen(L"invoices.dat", L"rb");
    if (!file) {
        wprintf(L"Не вдалося відкрити invoices.dat для читання.\n");
        return 0;
    }

    int count = 0;
    while (fread(&invoices[count], sizeof(Invoice), 1, file) == 1) {
        count++;
        if (count >= MAX_INVOICES) break;
    }

    fclose(file);
    return count;
}

int get_next_act_id() {
    FILE* file = _wfopen(L"last_act_id.txt", L"r, ccs=UNICODE");
    int last_id = 0;

    if (file) {
        fwscanf(file, L"%d", &last_id);
        fclose(file);
    }

    int next_id = last_id + 1;

    // Записуємо новий ID назад
    file = _wfopen(L"last_act_id.txt", L"w, ccs=UNICODE");
    if (file) {
        fwprintf(file, L"%d", next_id);
        fclose(file);
    }

    return next_id;
}

void create_act() {

    Invoice invoices[MAX_INVOICES];
    int invoice_count = load_invoices(invoices);

    load_partners();

    int selected_invoice_id;
    wprintf(L"\nВведіть ID інвойса, для якого створюється акт: ");
    wscanf(L"%d", &selected_invoice_id);
    while (getwchar() != L'\n');

    Invoice* invoice = NULL;
    for (int i = 0; i < invoice_count; i++) {
        if (invoices[i].order_id == selected_invoice_id) {
            invoice = &invoices[i];
            break;
        }
    }

    if (!invoice) {
        wprintf(L"Інвойс не знайдено.\n");
        return;
    }

    Partner* customer = NULL;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id == invoice->customer_id) {
            customer = &partners[i];
            break;
        }
    }

    if (!customer) {
        wprintf(L"Клієнт не знайдений.\n");
        return;
    }

    // Введення дати вручну
    wchar_t act_date[20];
    wprintf(L"Введіть дату складання акту (YYYY-MM-DD): ");
    fgetws(act_date, 20, stdin);
    act_date[wcslen(act_date) - 1] = L'\0';

    int act_id = get_next_act_id();

    // Формування акту
    wchar_t filename[100];
    swprintf(filename, 100, L"Acts/Act_%d.txt", act_id);
    _wmkdir(L"Acts");

    FILE* file = _wfopen(filename, L"w, ccs=UTF-8");
    if (!file) {
        wprintf(L"Не вдалося створити файл акту.\n");
        return;
    }

    wchar_t first_initial = customer->first_name[0];
    wchar_t fathers_initial = customer->fathers_name[0];

    fwprintf(file,
        L"\t\t\t\tАКТ надання послуг №%d від %ls\n\n"
        

        L"Ми, що нижче підписалися, представник Замовника %ls,\n"
        L"%ls %ls %ls %ls, з одного боку,\n"
        L"і представник Виконавця ТОВ \"ІНТАСК\", Директор Даценко Сергій Миколайович, з іншого боку,\n"
        L"склали цей акт про те, що на підставі наведених документів:\n\n"

        L"Договір: %ls № %ls від %ls\n"
        L"Розрахунковий документ: Рахунок №%ls від %ls\n\n"

        L"Виконавцем були надані такі послуги:\n"
        L"%ls. \n\n"

        L"Загальна вартість послуг склала: %.2f грн, без ПДВ\n"
        L"Замовник претензій по об'єму, якості та строкам надання послуг не має.\n\n"
        L"Місце складання: м. Бориспіль\n\n"

        L"Від Виконавця:\t\t\t\tВід Замовника:\n\n\n\n"
        L"Директор Даценко С.М.\t\t\t\t %ls %ls %lc.%lc.\n"
        L"ТОВ \"ІНТАСК\",\t\t\t\t\t%ls\n"
        L"код за ЄДРПОУ 37243792,\t\t\t\tКод за ЄДРПОУ: %ls\n"
        L"08303, Київська обл., м. Бориспіль,\t\tАдреса: %ls\n"
        L"вул.Нова, буд. 4, кв.72,\n"
        L"п/р UA343052990000026009006804903,\n"
        L"у банку АТ КБ \"Приватбанк\", МФО 305299,\n"
        L"тел.: 0673747000,\n"
        L"Платник єдиного податку 3 група 5%%\n\n",

        act_id, act_date,
        customer->name, customer->position, customer->last_name, customer->first_name, customer->fathers_name,
        customer->details, customer->contract_number, customer->start_date,
        invoice->invoice_number, invoice->invoice_date,
        invoice->service_name,
        invoice->amount_due,
        customer->position, customer->last_name, first_initial, fathers_initial, customer->name,
        customer->tax_code, customer->address
    );

    fclose(file);
    wprintf(L"Акт успішно створено: %ls\n", filename);
}

void view_acts() {
    _wmkdir(L"Acts"); // Створити папку, якщо не існує

    struct _wfinddata_t file_data;
    intptr_t handle;
    handle = _wfindfirst(L"Acts\\*.txt", &file_data); // Пошук .txt файлів у папці Acts

    if (handle == -1) {
        wprintf(L"Немає жодного акту для перегляду.\n");
        return;
    }

    do {
        wchar_t filepath[260];
        swprintf(filepath, 260, L"Acts\\%ls", file_data.name);

        FILE* file = _wfopen(filepath, L"r, ccs=UTF-8");
        if (file) {
            wprintf(L"\n=== %ls ===\n", file_data.name);

            wchar_t line[512];
            while (fgetws(line, sizeof(line) / sizeof(wchar_t), file)) {
                wprintf(L"%ls", line);
            }

            fclose(file);
        }
        else {
            wprintf(L"Не вдалося відкрити файл: %ls\n", filepath);
        }

    } while (_wfindnext(handle, &file_data) == 0);

    _findclose(handle);
}

Payment payments[MAX_PAYMENTS];
int payment_count = 0;

void save_payments() {
    FILE* f = _wfopen(L"data/payments.dat", L"wb");
    if (!f) return;
    fwrite(&payment_count, sizeof(int), 1, f);
    fwrite(payments, sizeof(Payment), payment_count, f);
    fclose(f);
}

int load_payments() {
    FILE* f = _wfopen(L"data/payments.dat", L"rb");
    if (!f) return 0;
    fread(&payment_count, sizeof(int), 1, f);
    fread(payments, sizeof(Payment), payment_count, f);
    fclose(f);
    return payment_count;
}

int get_next_payment_id() {
    FILE* file = _wfopen(L"payments.txt", L"r, ccs=UTF-8");
    if (!file) {
        // Якщо файл не існує — повертаємо ID = 1
        return 1;
    }

    int max_id = 0;
    wchar_t line[512];
    while (fgetws(line, sizeof(line) / sizeof(wchar_t), file)) {
        int id = 0;
        if (swscanf(line, L"{\"payment_id\": %d,", &id) == 1) {
            if (id > max_id) {
                max_id = id;
            }
        }
    }

    fclose(file);
    return max_id + 1;
}

void ensure_payments_file_exists() {
    FILE* file = _wfopen(L"payments.txt", L"a, ccs=UTF-8"); // "a" створить, якщо не існує
    if (file) fclose(file);
}


void create_payment() {
    Invoice invoices[MAX_INVOICES];
    int invoice_count = load_invoices(invoices);

    load_partners();

    int selected_invoice_id;
    wprintf(L"\nВведіть ID інвойса, на основі якого створюється платіж: ");
    wscanf(L"%d", &selected_invoice_id);
    while (getwchar() != L'\n');

    Invoice* invoice = NULL;
    for (int i = 0; i < invoice_count; i++) {
        if (invoices[i].invoice_id == selected_invoice_id) {
            invoice = &invoices[i];
            break;
        }
    }

    if (!invoice) {
        wprintf(L"Інвойс не знайдено.\n");
        return;
    }

    Partner* partner = NULL;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id == invoice->customer_id) {
            partner = &partners[i];
            break;
        }
    }

    if (!partner) {
        wprintf(L"Партнера за інвойсом не знайдено.\n");
        return;
    }

    wchar_t payment_type[20];
    wprintf(L"Введіть тип платежу (from_customer / to_performer): ");
    fgetws(payment_type, 20, stdin);
    payment_type[wcslen(payment_type) - 1] = L'\0';

    wchar_t payment_date[20];
    wprintf(L"Введіть дату платежу (YYYY-MM-DD): ");
    fgetws(payment_date, 20, stdin);
    payment_date[wcslen(payment_date) - 1] = L'\0';

    double amount_paid;
    wprintf(L"Введіть суму платежу: ");
    wscanf(L"%lf", &amount_paid);
    while (getwchar() != L'\n');

    int payment_id = get_next_payment_id();

    FILE* file = _wfopen(L"payments.txt", L"a, ccs=UTF-8");
    if (!file) {
        wprintf(L"Не вдалося відкрити файл для запису платежу.\n");
        return;
    }

    fwprintf(file, L"{\"payment_id\": %d, \"payment_type\": \"%ls\", \"invoice_id\": %d, \"partner_id\": %d, \"amount_paid\": %.2f, \"payment_date\": \"%ls\"}\n",
        payment_id, payment_type, invoice->invoice_id, partner->id, amount_paid, payment_date);

    fclose(file);

    wprintf(L"Платіж успішно збережено з ID: %d\n", payment_id);
}


void view_payments() {
    FILE* file = _wfopen(L"payments.txt", L"r, ccs=UTF-8");
    if (!file) {
        wprintf(L"Файл з платежами не знайдено.\n");
        return;
    }

    wchar_t line[512];
    wprintf(L"\n=== Перелік платежів ===\n");
    while (fgetws(line, sizeof(line) / sizeof(wchar_t), file)) {
        int payment_id, invoice_id, partner_id;
        double amount_paid;
        wchar_t payment_type[20], payment_date[20];

        if (swscanf(line,
            L"{\"payment_id\": %d, \"payment_type\": \"%19[^\"]\", \"invoice_id\": %d, \"partner_id\": %d, \"amount_paid\": %lf, \"payment_date\": \"%19[^\"]\"}",
            &payment_id, payment_type, &invoice_id, &partner_id, &amount_paid, payment_date) == 6) {

            wprintf(L"\nПлатіж ID: %d\nТип: %ls\nІнвойс ID: %d\nПартнер ID: %d\nСума: %.2f грн\nДата: %ls\n",
                payment_id, payment_type, invoice_id, partner_id, amount_paid, payment_date);
        }
    }

    fclose(file);
}



void run_menu() {
    wchar_t input[10];
    int choice = -1;

    do {
        wprintf(L"\n\t\t\t\t\t\t=== Головне меню ===\n\n");
        wprintf(L"1. Додати партнера\t\t4. Додати послугу\t6. Додати замовлення\t\t10. Сформувати рахунок\n");
        wprintf(L"2. Переглянути партнерів\t5. Переглянути послуги\t7. Переглянути замовлення\t11. Переглянути рахунки\n");
        wprintf(L"3. Видалити партнера\t\t\t\t\t8. Видалити замовлення\t\t12. Створити акт\n");
        wprintf(L"\t\t\t\t\t\t\t9. Редагувати замовлення\t13. Переглянути акти\n\n");
        wprintf(L"14. Додати платіж\n");
        wprintf(L"15. Перегляд платежів\n\n");
        wprintf(L"0. Вийти\n\n");
        wprintf(L"Ваш вибір: ");

        fgetws(input, 10, stdin);
        swscanf(input, L"%d", &choice);

        switch (choice) {
        case 1: add_partner(); break;
        case 2: list_partners(); break;
        case 3: delete_partner(); break; 
        case 4: add_service(); break;
        case 5: list_services(); break;
        case 6: add_order(); break;
        case 7:
            load_partners();
            load_services();
            order_count = load_orders(orders);
            list_orders(orders, order_count, partners, partner_count, services, service_count);
            break;
            case 8:
            wprintf(L"Введіть ID замовлення для видалення: ");
            int del_id;
            wscanf(L"%d", &del_id);
            while (getwchar() != L'\n'); // очищення буфера
            delete_order_by_id(del_id);
            break;
        case 9: edit_order(); break;
        case 10: create_invoice(); break;
        case 11: list_invoices(); break;
        case 12:
            int load_invoices(Invoice invoices[]);
            create_act(); break;
        case 13:
            view_acts();
            break;
        case 14:
            ensure_payments_file_exists();
            create_payment();
            break;
        case 15:
            view_payments();
            break;
        case 0: wprintf(L"Завершення програми...\n"); break;
        default: wprintf(L"Невірний вибір. Спробуйте ще раз.\n");
        }

    } while (choice != 0);
}

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _wsetlocale(LC_ALL, L"uk_UA.UTF-8");

    run_menu();
    return 0;
}
