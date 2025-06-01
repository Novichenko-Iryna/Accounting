#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LEN 100
#define MAX_ORDERS 1000
#define MAX_PARTNERS 100
#define MAX_SERVICES 100
#define MAX_CONTRACTS 100
#define MAX_INVOICES 1000
#define MAX_PAYMENTS 1000
#define MAX_ACTS 1000
#define MAX_ROWS 1000
#define MAX_EMPLOYEES 100
#define INVOICE_FROM_CUSTOMER 0
#define INVOICE_TO_PERFORMER 1
#define PAYMENT_TYPE_FROM_CUSTOMER 0
#define PAYMENT_TYPE_TO_PERFORMER 1


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
    int invoice_type; // 0 - рахунок замовнику, 1 - рахунок від виконавця
    wchar_t invoice_number[20];
    int order_id;
    int customer_id;
    int service_id;
    wchar_t service_name[MAX_LEN];
    double amount_due;
    wchar_t invoice_date[20];
} Invoice;

typedef struct {
    int id;
    wchar_t act_number[20];     // Номер акту (наприклад, "A-001")
    wchar_t date[20];           // Дата створення акту
    int order_id;               // ID замовлення
    int partner_id;             // ID партнера (замовника або перевізника)
    wchar_t act_type[20];       // "замовнику" або "перевізнику"
} Act;

typedef struct {
    int payment_id;
    wchar_t payment_type[20]; // "from_customer" або "to_performer"
    int order_id;
    int partner_id;
    double amount_paid;
    wchar_t payment_date[20];
} Payment;

typedef struct {
    int id;                    // Унікальний ідентифікатор працівника
    wchar_t last_name[50];     // Прізвище
    wchar_t first_name[50];    // Ім’я
    wchar_t fathers_name[50];  // По батькові 
    wchar_t gender[10];        // Стать ("Чоловіча" або "Жіноча")
    wchar_t tax_number[20];    // Індивідуальний податковий номер (ІПН)
    wchar_t position[50];      // Посада працівника
} Employee;

typedef struct {
    Order order;
    double margin_value;
} OrderWithMargin;

typedef struct {
    int partner_id;
    int order_id;
    double dt;
    double kt;
} Row;

void clear_input_buffer() {
    wchar_t ch;
    while ((ch = getwchar()) != L'\n' && ch != WEOF); // читання до кінця рядка
}

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

Order* find_order_by_id(int id, Order* orders, int order_count) {
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == id)
            return &orders[i];
    }
    return NULL;
}

Partner partners[MAX_PARTNERS];
int partner_count = 0;

Service services[MAX_SERVICES];
int service_count;

Order orders[MAX_ORDERS];
int order_count;

Payment payments[MAX_PAYMENTS];
int payment_count = 0;

Invoice invoices[MAX_INVOICES];
int invoice_count = 0;

Act acts[MAX_ACTS];
int act_count = 0;

Employee employees[MAX_EMPLOYEES];
int employee_count = 0;

// --- Довідники ----

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

void add_partner() {
    // 1) Завантажуємо наявних партнерів
    load_partners();  // заповнить partners[] і partner_count

    if (partner_count >= MAX_PARTNERS) {
        wprintf(L"⚠️ Максимальна кількість партнерів досягнута.\n");
        return;
    }

    Partner p = { 0 };

    // 2) Генеруємо унікальний ID
    int max_id = 0;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id > max_id) max_id = partners[i].id;
    }
    p.id = max_id + 1;

    wprintf(L"\n=== Додавання партнера (ID=%d) ===\n", p.id);

    wprintf(L"Роль (Customer/Performer): ");
    fgetws(p.role, sizeof(p.role) / sizeof(wchar_t), stdin); trim_newline(p.role);

    wprintf(L"Назва компанії: ");
    fgetws(p.name, sizeof(p.name) / sizeof(wchar_t), stdin); trim_newline(p.name);

    wprintf(L"Прізвище керівника: ");
    fgetws(p.last_name, sizeof(p.last_name) / sizeof(wchar_t), stdin); trim_newline(p.last_name);

    wprintf(L"Ім'я керівника: ");
    fgetws(p.first_name, sizeof(p.first_name) / sizeof(wchar_t), stdin); trim_newline(p.first_name);

    wprintf(L"По батькові керівника: ");
    fgetws(p.fathers_name, sizeof(p.fathers_name) / sizeof(wchar_t), stdin); trim_newline(p.fathers_name);

    wprintf(L"Посада керівника: ");
    fgetws(p.position, sizeof(p.position) / sizeof(wchar_t), stdin); trim_newline(p.position);

    wprintf(L"Адреса: ");
    fgetws(p.address, sizeof(p.address) / sizeof(wchar_t), stdin); trim_newline(p.address);

    wprintf(L"ІПН (код): ");
    fgetws(p.tax_code, sizeof(p.tax_code) / sizeof(wchar_t), stdin); trim_newline(p.tax_code);

    wprintf(L"Дата початку контракту (YYYY-MM-DD): ");
    fgetws(p.start_date, sizeof(p.start_date) / sizeof(wchar_t), stdin); trim_newline(p.start_date);

    wprintf(L"Номер контракту: ");
    fgetws(p.contract_number, sizeof(p.contract_number) / sizeof(wchar_t), stdin); trim_newline(p.contract_number);

    wprintf(L"Деталі (опис): ");
    fgetws(p.details, sizeof(p.details) / sizeof(wchar_t), stdin); trim_newline(p.details);

    // 3) Додаємо в масив
    partners[partner_count++] = p;

    // 4) Перезаписуємо увесь файл partners.txt
    FILE* f = _wfopen(L"partners.txt", L"w, ccs=UNICODE");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити partners.txt для запису.\n");
        return;
    }
    for (int i = 0; i < partner_count; i++) {
        Partner* q = &partners[i];
        fwprintf(f,
            L"%d;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls;%ls\n",
            q->id,
            q->role,
            q->name,
            q->last_name,
            q->first_name,
            q->fathers_name,
            q->position,
            q->address,
            q->tax_code,
            q->start_date,
            q->contract_number,
            q->details
        );
    }
    fclose(f);

    wprintf(L"✅ Партнера ID=%d додано успішно.\n", p.id);
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

void edit_partner() {
    // 1) Завантажуємо поточні дані
    load_partners();
    if (partner_count == 0) {
        wprintf(L"⚠️ Немає партнерів для редагування.\n");
        return;
    }

    // 2) Вибір ID
    int id;
    wprintf(L"Введіть ID партнера для редагування: ");
    wscanf(L"%d", &id);
    clear_input_buffer();

    // 3) Знаходимо індекс у масиві
    int idx = -1;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id == id) { idx = i; break; }
    }
    if (idx < 0) {
        wprintf(L"❌ Партнера з ID=%d не знайдено.\n", id);
        return;
    }

    Partner* p = &partners[idx];
    wprintf(L"\n--- Редагування партнера ID=%d ---\n", p->id);

    // 4) Редагуємо поля (як у вас було)
    wprintf(L"Роль (Customer/Performer) [%ls]: ", p->role);
    fgetws(p->role, sizeof(p->role) / sizeof(wchar_t), stdin); trim_newline(p->role);
    wprintf(L"Назва компанії [%ls]: ", p->name);
    fgetws(p->name, sizeof(p->name) / sizeof(wchar_t), stdin); trim_newline(p->name);
    wprintf(L"Прізвище керівника [%ls]: ", p->last_name);
    fgetws(p->last_name, sizeof(p->last_name) / sizeof(wchar_t), stdin); trim_newline(p->last_name);
    wprintf(L"Ім'я керівника [%ls]: ", p->first_name);
    fgetws(p->first_name, sizeof(p->first_name) / sizeof(wchar_t), stdin); trim_newline(p->first_name);
    wprintf(L"По батькові [%ls]: ", p->fathers_name);
    fgetws(p->fathers_name, sizeof(p->fathers_name) / sizeof(wchar_t), stdin); trim_newline(p->fathers_name);
    wprintf(L"Посада [%ls]: ", p->position);
    fgetws(p->position, sizeof(p->position) / sizeof(wchar_t), stdin); trim_newline(p->position);
    wprintf(L"Адреса [%ls]: ", p->address);
    fgetws(p->address, sizeof(p->address) / sizeof(wchar_t), stdin); trim_newline(p->address);
    wprintf(L"ІПН [%ls]: ", p->tax_code);
    fgetws(p->tax_code, sizeof(p->tax_code) / sizeof(wchar_t), stdin); trim_newline(p->tax_code);
    wprintf(L"Дата початку контракту [%ls]: ", p->start_date);
    fgetws(p->start_date, sizeof(p->start_date) / sizeof(wchar_t), stdin); trim_newline(p->start_date);
    wprintf(L"Номер контракту [%ls]: ", p->contract_number);
    fgetws(p->contract_number, sizeof(p->contract_number) / sizeof(wchar_t), stdin); trim_newline(p->contract_number);
    wprintf(L"Деталі [%ls]: ", p->details);
    fgetws(p->details, sizeof(p->details) / sizeof(wchar_t), stdin); trim_newline(p->details);

    // 5) Запис у тимчасовий файл
    FILE* tmp = _wfopen(L"partners.tmp", L"w, ccs=UNICODE");
    if (!tmp) {
        wprintf(L"❌ Не вдалося відкрити тимчасовий файл для запису.\n");
        return;
    }
    for (int i = 0; i < partner_count; i++) {
        Partner* q = &partners[i];
        fwprintf(tmp,
            L"%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
            q->id,
            q->role,
            q->name,
            q->last_name,
            q->first_name,
            q->fathers_name,
            q->position,
            q->address,
            q->tax_code,
            q->start_date,
            q->contract_number,
            q->details
        );
    }
    fclose(tmp);

    // 6) Замінюємо старий файл
    if (_wremove(L"partners.txt") != 0 || _wrename(L"partners.tmp", L"partners.txt") != 0) {
        wprintf(L"❌ Помилка при заміні файлу partners.txt\n");
        return;
    }

    wprintf(L"✅ Партнера ID=%d успішно оновлено.\n", id);
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

void delete_service_by_id(int target_id) {
    FILE* in = _wfopen(L"services.dat", L"rb");
    FILE* out = _wfopen(L"temp.dat", L"wb");
    if (!in || !out) {
        wprintf(L"❌ Не вдалося відкрити файли для обробки послуг.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    Service s;
    int found = 0;
    while (fread(&s, sizeof(Service), 1, in) == 1) {
        if (s.id != target_id) {
            fwrite(&s, sizeof(Service), 1, out);
        }
        else {
            found = 1;
        }
    }
    fclose(in);
    fclose(out);

    if (found) {
        _wremove(L"services.dat");
        _wrename(L"temp.dat", L"services.dat");
        wprintf(L"✅ Послугу ID=%d видалено.\n", target_id);
    }
    else {
        _wremove(L"temp.dat");
        wprintf(L"⚠️ Послугу ID=%d не знайдено.\n", target_id);
    }
}

void delete_service_prompt() {
    int id;
    wprintf(L"Введіть ID послуги для видалення: ");
    wscanf(L"%d", &id);
    clear_input_buffer();
    delete_service_by_id(id);
}

void edit_service() {
    load_services();
    if (service_count == 0) {
        wprintf(L"⚠️ Немає послуг для редагування.\n");
        return;
    }

    int id;
    wprintf(L"Введіть ID послуги для редагування: ");
    wscanf(L"%d", &id);
    clear_input_buffer();

    // Шукаємо
    int idx = -1;
    for (int i = 0; i < service_count; i++) {
        if (services[i].id == id) { idx = i; break; }
    }
    if (idx < 0) {
        wprintf(L"❌ Послугу з ID=%d не знайдено.\n", id);
        return;
    }

    Service* s = &services[idx];
    wprintf(L"\n--- Редагування послуги ID=%d ---\n", s->id);

    wprintf(L"Тип (Received/Provided) [%ls]: ", s->appointment);
    fgetws(s->appointment, 20, stdin); trim_newline(s->appointment);

    wprintf(L"Назва [%ls]: ", s->service_name);
    fgetws(s->service_name, MAX_LEN, stdin); trim_newline(s->service_name);

    wprintf(L"Бухгалтерський рахунок [%ls]: ", s->balance_account);
    fgetws(s->balance_account, 20, stdin); trim_newline(s->balance_account);

    // Перезаписуємо весь services.dat
    FILE* f = _wfopen(L"services.dat", L"wb");
    for (int i = 0; i < service_count; i++) {
        fwrite(&services[i], sizeof(Service), 1, f);
    }
    fclose(f);

    wprintf(L"✅ Послугу ID=%d оновлено.\n", id);
}

//  --- Замовлення --- 

int get_next_order_id(const Order* orders, int order_count) {
    int max_id = 0;
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id > max_id) {
            max_id = orders[i].id;
        }
    }
    return max_id + 1;
}

void save_orders(const Order* orders, int order_count) {
    FILE* f = _wfopen(L"orders.dat", L"wb");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити orders.dat для запису.\n");
        return;
    }
    fwrite(orders, sizeof(Order), order_count, f);
    fclose(f);
}

int load_orders(Order* orders) {
    // Спроба відкрити для читання
    FILE* f = _wfopen(L"orders.dat", L"rb");
    if (!f) {
        // Якщо нема — створюємо порожній файл
        FILE* wf = _wfopen(L"orders.dat", L"wb");
        if (wf) fclose(wf);
        return 0;
    }

    int count = 0;
    while (count < MAX_ORDERS && fread(&orders[count], sizeof(Order), 1, f) == 1) {
        count++;
    }
    fclose(f);
    return count;
}

void add_order() {
    // 1️⃣ Завантажуємо існуючі замовлення
    order_count = load_orders(orders);

    if (order_count >= MAX_ORDERS) {
        wprintf(L"⚠️ Досягнуто максимальну кількість замовлень.\n");
        return;
    }

    Order o = { 0 };

    // 2️⃣ Генеруємо унікальний ID
    o.id = get_next_order_id(orders, order_count);

    // 3️⃣ Запитуємо решту полів у користувача
    wprintf(L"\n--- Додавання замовлення (ID = %d) ---\n", o.id);

    wprintf(L"ID замовника: ");
    wscanf(L"%d", &o.customer_id);
    clear_input_buffer();

    wprintf(L"ID виконавця: ");
    wscanf(L"%d", &o.performer_id);
    clear_input_buffer();

    wprintf(L"ID послуги: ");
    wscanf(L"%d", &o.service_id);
    clear_input_buffer();

    wprintf(L"Дата замовлення (YYYY-MM-DD): ");
    fgetws(o.date_of_order, sizeof(o.date_of_order) / sizeof(wchar_t), stdin);
    trim_newline(o.date_of_order);

    wprintf(L"Місце завантаження: ");
    fgetws(o.loading_place, sizeof(o.loading_place) / sizeof(wchar_t), stdin);
    trim_newline(o.loading_place);

    wprintf(L"Місце розвантаження: ");
    fgetws(o.unloading_place, sizeof(o.unloading_place) / sizeof(wchar_t), stdin);
    trim_newline(o.unloading_place);

    wprintf(L"Вартість для замовника: ");
    wscanf(L"%lf", &o.freight);
    clear_input_buffer();

    wprintf(L"Вартість від виконавця: ");
    wscanf(L"%lf", &o.margin);
    clear_input_buffer();

    wprintf(L"Номер автомобіля: ");
    fgetws(o.vehicle_number, sizeof(o.vehicle_number) / sizeof(wchar_t), stdin);
    trim_newline(o.vehicle_number);

    wprintf(L"Дата розвантаження (YYYY-MM-DD): ");
    fgetws(o.date_of_unloading, sizeof(o.date_of_unloading) / sizeof(wchar_t), stdin);
    trim_newline(o.date_of_unloading);

    o.invoice_issued = 0;

    // 4️⃣ Додаємо в масив і зберігаємо всі замовлення в файл
    orders[order_count++] = o;
    save_orders(orders, order_count);

    wprintf(L"✅ Замовлення #%d успішно додано!\n", o.id);
}

void list_orders(
    Order* orders, int order_count,
    Partner* partners, int partner_count,
    Service* services, int service_count,
    const wchar_t* from, const wchar_t* to
) {
    wprintf(L"\n=== Список замовлень (з %ls по %ls) ===\n", from, to);
    int found = 0;

    for (int i = 0; i < order_count; i++) {
        // Фільтруємо за date_of_order
        const wchar_t* d = orders[i].date_of_order;
        if (wcscmp(d, from) < 0 || wcscmp(d, to) > 0) {
            continue;
        }

        found = 1;
        wprintf(L"\nЗамовлення ID: %d\n", orders[i].id);

        Partner* customer = find_partner_by_id(partners, partner_count, orders[i].customer_id);
        Partner* performer = find_partner_by_id(partners, partner_count, orders[i].performer_id);
        Service* service = find_service_by_id(services, service_count, orders[i].service_id);

        if (customer) {
            wprintf(L"  Замовник: %ls\n", customer->name);
        }
        else {
            wprintf(L"  Замовник: [ID %d не знайдено]\n", orders[i].customer_id);
        }

        if (performer) {
            wprintf(L"  Виконавець: %ls\n", performer->name);
        }
        else {
            wprintf(L"  Виконавець: [ID %d не знайдено]\n", orders[i].performer_id);
        }

        if (service) {
            wprintf(L"  Послуга: %ls (%ls)\n", service->service_name, service->appointment);
        }
        else {
            wprintf(L"  Послуга: [ID %d не знайдено]\n", orders[i].service_id);
        }

        wprintf(L"  Дата замовлення:      %ls\n", orders[i].date_of_order);
        wprintf(L"  Місце завантаження:   %ls\n", orders[i].loading_place);
        wprintf(L"  Місце розвантаження:  %ls\n", orders[i].unloading_place);
        wprintf(L"  Вартість для клієнта:  %.2lf\n", orders[i].freight);
        wprintf(L"  Вартість від перевізника: %.2lf\n", orders[i].margin);
        wprintf(L"  Номер авто:           %ls\n", orders[i].vehicle_number);
        wprintf(L"  Дата розвантаження:   %ls\n", orders[i].date_of_unloading);
        wprintf(L"  ------------------------\n");
    }

    if (!found) {
        wprintf(L"⚠️ За вказаний період (%ls – %ls) замовлень не знайдено.\n", from, to);
    }
    wprintf(L"\n");
}
void delete_order_by_id(int delete_id) {
    // 1) Завантажуємо всі замовлення
    int count = load_orders(orders);

    // 2) Фільтруємо масив
    int new_count = 0;
    for (int i = 0; i < count; i++) {
        if (orders[i].id != delete_id) {
            orders[new_count++] = orders[i];
        }
    }

    if (new_count == count) {
        wprintf(L"⚠️ Замовлення з ID %d не знайдено.\n", delete_id);
        return;
    }

    // 3) Зберігаємо оновлений масив
    save_orders(orders, new_count);
    order_count = new_count;

    wprintf(L"✅ Замовлення з ID %d успішно видалено.\n", delete_id);
}

void edit_order() {
    // 1) Завантажуємо дані
    order_count = load_orders(orders);
    load_partners();
    load_services();

    if (order_count == 0) {
        wprintf(L"⚠️ Немає замовлень для редагування.\n");
        return;
    }

    int id;
    wprintf(L"\nВведіть ID замовлення для редагування: ");
    wscanf(L"%d", &id);
    clear_input_buffer();

    // 2) Знаходимо індекс
    int idx = -1;
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        wprintf(L"⚠️ Замовлення з ID %d не знайдено.\n", id);
        return;
    }

    Order* o = &orders[idx];
    wprintf(L"\n--- Редагування замовлення ID %d ---\n", o->id);

    wprintf(L"Нове місце завантаження (поточне: %ls): ", o->loading_place);
    fgetws(o->loading_place, sizeof(o->loading_place) / sizeof(wchar_t), stdin);
    trim_newline(o->loading_place);

    wprintf(L"Нове місце розвантаження (поточне: %ls): ", o->unloading_place);
    fgetws(o->unloading_place, sizeof(o->unloading_place) / sizeof(wchar_t), stdin);
    trim_newline(o->unloading_place);

    wprintf(L"Нова вартість для замовника (%.2f): ", o->freight);
    wscanf(L"%lf", &o->freight);
    clear_input_buffer();

    wprintf(L"Нова вартість від виконавця (%.2f): ", o->margin);
    wscanf(L"%lf", &o->margin);
    clear_input_buffer();

    wprintf(L"Новий номер автомобіля (поточний: %ls): ", o->vehicle_number);
    fgetws(o->vehicle_number, sizeof(o->vehicle_number) / sizeof(wchar_t), stdin);
    trim_newline(o->vehicle_number);

    wprintf(L"Нова дата розвантаження (YYYY-MM-DD, поточна: %ls): ", o->date_of_unloading);
    fgetws(o->date_of_unloading, sizeof(o->date_of_unloading) / sizeof(wchar_t), stdin);
    trim_newline(o->date_of_unloading);

    // Оновлюємо клієнта
    wprintf(L"\n--- Список клієнтів ---\n");
    for (int i = 0; i < partner_count; i++)
        wprintf(L"ID: %d — %ls\n", partners[i].id, partners[i].name);
    wprintf(L"Новий ID клієнта (поточний: %d): ", o->customer_id);
    wscanf(L"%d", &o->customer_id);
    clear_input_buffer();

    // Оновлюємо виконавця
    wprintf(L"\n--- Список виконавців ---\n");
    for (int i = 0; i < partner_count; i++)
        wprintf(L"ID: %d — %ls\n", partners[i].id, partners[i].name);
    wprintf(L"Новий ID виконавця (поточний: %d): ", o->performer_id);
    wscanf(L"%d", &o->performer_id);
    clear_input_buffer();

    // Оновлюємо послугу
    wprintf(L"\n--- Список послуг ---\n");
    for (int i = 0; i < service_count; i++)
        wprintf(L"ID: %d — %ls\n", services[i].id, services[i].service_name);
    wprintf(L"Новий ID послуги (поточний: %d): ", o->service_id);
    wscanf(L"%d", &o->service_id);
    clear_input_buffer();

    // 3) Зберігаємо всі замовлення назад на диск
    save_orders(orders, order_count);

    wprintf(L"\n✅ Замовлення ID %d оновлено.\n", id);
}

void import_orders_csv() {
    // 1) Запитуємо шлях
    wchar_t path[260];
    wprintf(L"\nВведіть шлях до CSV-файлу (наприклад C:\\data\\orders.csv):\n> ");
    fgetws(path, 260, stdin);
    trim_newline(path);

    // 2) Завантажуємо існуючі замовлення
    order_count = load_orders(orders);

    // 3) Відкриваємо CSV
    FILE* f = _wfopen(path, L"r, ccs=UTF-8");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити файл:\n   %ls\n", path);
        return;
    }

    // 4) Читаємо та імпортуємо
    wchar_t line_mb[1024];
    char    line[1024];

    // Пропускаємо заголовок (ANSI)
    if (!fgetws(line_mb, 1024, f)) {
        fclose(f);
        return;
    }
    // Конвертуємо перший рядок у ANSI, бо strtok очікує char*
    wcstombs(line, line_mb, sizeof(line));

    while (fgetws(line_mb, 1024, f) && order_count < MAX_ORDERS) {
        // Конвертуємо у char[]
        wcstombs(line, line_mb, sizeof(line));
        line[strcspn(line, "\r\n")] = 0;

        char* tok = strtok(line, ",");
        if (!tok) continue;

        Order o = { 0 };
        o.id = get_next_order_id(orders, order_count);

        // 1) customer_id
        o.customer_id = atoi(tok);
        // 2) performer_id
        tok = strtok(NULL, ","); o.performer_id = atoi(tok);
        // 3) service_id
        tok = strtok(NULL, ","); o.service_id = atoi(tok);
        // 4–6) Year,Month,Day
        tok = strtok(NULL, ","); int year = atoi(tok);
        tok = strtok(NULL, ","); int month = atoi(tok);
        tok = strtok(NULL, ","); int day = atoi(tok);
        swprintf(o.date_of_order, 20, L"%04d-%02d-%02d", year, month, day);
        swprintf(o.date_of_unloading, 20, L"%04d-%02d-%02d", year, month, day);
        // 7) Load
        tok = strtok(NULL, ","); mbstowcs(o.loading_place, tok, 100);
        // 8) Unload
        tok = strtok(NULL, ","); mbstowcs(o.unloading_place, tok, 100);
        // 9) Vehicle_number
        tok = strtok(NULL, ","); mbstowcs(o.vehicle_number, tok, 20);
        // 10) Freight
        tok = strtok(NULL, ","); o.freight = atof(tok);
        // 11) Margin
        tok = strtok(NULL, ","); o.margin = atof(tok);
        o.invoice_issued = 0;

        orders[order_count++] = o;
    }

    fclose(f);

    // 5) Зберігаємо оновлений масив
    save_orders(orders, order_count);

    wprintf(L"✅ Імпортовано замовлень: %d\n", order_count);
}

// --- Фінансові документи ---

void generate_invoice_number(wchar_t* buffer, int max) {
    static int counter = 1; // в реальності краще зберігати це в окремому файлі
    swprintf(buffer, max, L"INV%05d", counter++);
}

int get_next_invoice_id() {
    // 1) Знайти max_id серед існуючих у invoices.dat
    Invoice tmp;
    int max_id = 0;
    FILE* f = _wfopen(L"invoices.dat", L"rb");
    if (f) {
        while (fread(&tmp, sizeof(tmp), 1, f) == 1) {
            if (tmp.invoice_id > max_id) {
                max_id = tmp.invoice_id;
            }
        }
        fclose(f);
    }

    // 2) Власне наступний
    int next_id = max_id + 1;

    // 3) Підтримати last_invoice_id.txt синхронним
    FILE* lf = _wfopen(L"last_invoice_id.txt", L"w, ccs=UNICODE");
    if (lf) {
        fwprintf(lf, L"%d", next_id);
        fclose(lf);
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

    Service* service = NULL;
    for (int i = 0; i < service_count; i++) {
        if (services[i].id == o->service_id) {
            service = &services[i];
            break;
        }
    }

    if (!service) {
        wprintf(L"Послугу не знайдено.\n");
        return;
    }

    int invoice_type_choice;
    wprintf(L"Оберіть тип рахунку:\n");
    wprintf(L"1. Рахунок для замовника\n");
    wprintf(L"2. Рахунок від виконавця\n");
    wprintf(L"Ваш вибір: ");
    wscanf(L"%d", &invoice_type_choice); while (getwchar() != L'\n');

    if (invoice_type_choice != 1 && invoice_type_choice != 2) {
        wprintf(L"Невірний вибір типу рахунку.\n");
        return;
    }

    Partner* partner = NULL;
    if (invoice_type_choice == 1) {
        for (int i = 0; i < partner_count; i++) {
            if (partners[i].id == o->customer_id) {
                partner = &partners[i];
                break;
            }
        }
    }
    else {
        for (int i = 0; i < partner_count; i++) {
            if (partners[i].id == o->performer_id) {
                partner = &partners[i];
                break;
            }
        }
    }

    if (!partner) {
        wprintf(L"Не вдалося знайти партнера для рахунку.\n");
        return;
    }

    Invoice inv;
    inv.invoice_id = get_next_invoice_id();
    generate_invoice_number(inv.invoice_number, 20);
    inv.invoice_type = (invoice_type_choice == 1) ? 0 : 1;
    inv.order_id = o->id;
    inv.customer_id = partner->id;
    inv.service_id = service->id;
    wcscpy(inv.service_name, service->service_name);

    // Визначення суми до сплати
    if (invoice_type_choice == 1) {
        inv.amount_due = o->freight;  // замовник платить повну вартість
    }
    else {
        inv.amount_due = o->margin;  // виконавцю — без націнки
    }

    wprintf(L"Введіть дату рахунку (YYYY-MM-DD): ");
    fgetws(inv.invoice_date, 20, stdin);
    inv.invoice_date[wcslen(inv.invoice_date) - 1] = L'\0';

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
    fwprintf(f, L"Покупець:\t\t\t%ls\n", partner->name);
    fwprintf(f, L"Згідно договору: %ls, №%ls від %ls\n\n", partner->details, partner->contract_number, partner->start_date);
    fwprintf(f, L"За %ls з %ls до %ls, а/м %ls\n", service->service_name, o->loading_place, o->unloading_place, o->vehicle_number);
    fwprintf(f, L"\nВсього найменувань 1, на суму %.2lf грн.\n\n\n\n\n", inv.amount_due);
    fwprintf(f, L"Рахунок виписав:\t\t\t Директор Даценко С.М.\n");
    fclose(f);

    FILE* df = _wfopen(L"invoices.dat", L"ab");
    if (!df) {
        wprintf(L"Не вдалося відкрити файл invoices.dat для запису.\n");
        return;
    }
    fwrite(&inv, sizeof(Invoice), 1, df);
    fclose(df);

    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == order_id) {
            orders[i].invoice_issued = 1;
            break;
        }
    }

    FILE* of = _wfopen(L"orders.dat", L"wb");
    if (of) {
        fwrite(orders, sizeof(Order), order_count, of);
        fclose(of);
    }

    wprintf(L"\nРахунок успішно сформовано: %ls\n", filename);
}

void list_invoices_period(
    Invoice* invoices, int invoice_count,
    const wchar_t* from, const wchar_t* to
) {
    wprintf(L"\n=== Список рахунків (з %ls по %ls) ===\n", from, to);
    int found = 0;

    for (int i = 0; i < invoice_count; i++) {
        const wchar_t* d = invoices[i].invoice_date;
        if (wcscmp(d, from) < 0 || wcscmp(d, to) > 0) {
            continue;
        }

        found = 1;
        wprintf(L"\nІнвойс ID: %d\n", invoices[i].invoice_id);
        wprintf(L"  Тип: %ls\n", invoices[i].invoice_type == 0 ? L"Рахунок замовнику" : L"Рахунок від виконавця");
        wprintf(L"  Номер рахунку: %ls\n", invoices[i].invoice_number);
        wprintf(L"  Замовлення ID: %d\n", invoices[i].order_id);
        wprintf(L"  Партнер (Customer_ID): %d\n", invoices[i].customer_id);
        wprintf(L"  Послуга: %ls (ID %d)\n", invoices[i].service_name, invoices[i].service_id);
        wprintf(L"  Сума до сплати: %.2lf грн\n", invoices[i].amount_due);
        wprintf(L"  Дата інвойсу: %ls\n", invoices[i].invoice_date);
        wprintf(L"  ------------------------\n");
    }

    if (!found) {
        wprintf(L"⚠️ За вказаний період (%ls – %ls) інвойсів не знайдено.\n", from, to);
    }
    wprintf(L"\n");
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
    Act tmp;
    int max_id = 0;
    FILE* f = _wfopen(L"acts.dat", L"rb");
    if (f) {
        while (fread(&tmp, sizeof(tmp), 1, f) == 1) {
            if (tmp.id > max_id) max_id = tmp.id;
        }
        fclose(f);
    }
    int next = max_id + 1;
    FILE* lf = _wfopen(L"last_act_id.txt", L"w, ccs=UNICODE");
    if (lf) { fwprintf(lf, L"%d", next); fclose(lf); }
    return next;
}

void create_act() {
    Order orders[MAX_ORDERS];
    int order_count = load_orders(orders);
    load_partners();

    int selected_order_id;
    wprintf(L"\nВведіть ID замовлення, для якого створюється акт: ");
    wscanf(L"%d", &selected_order_id);
    while (getwchar() != L'\n');

    Order* order = NULL;
    for (int i = 0; i < order_count; i++) {
        if (orders[i].id == selected_order_id) {
            order = &orders[i];
            break;
        }
    }

    if (!order) {
        wprintf(L"Замовлення не знайдено.\n");
        return;
    }

    wchar_t act_type[20];
    wprintf(L"Кому створюється акт? (замовнику/перевізнику): ");
    fgetws(act_type, 20, stdin);
    act_type[wcslen(act_type) - 1] = L'\0';

    int partner_id;
    double amount;
    if (wcscmp(act_type, L"перевізнику") == 0) {
        partner_id = order->performer_id;
        amount = order->margin;
    }
    else if (wcscmp(act_type, L"замовнику") == 0) {
        partner_id = order->customer_id;
        amount = order->freight;
    }
    else {
        wprintf(L"Неправильний тип акту.\n");
        return;
    }

    Partner* partner = NULL;
    for (int i = 0; i < partner_count; i++) {
        if (partners[i].id == partner_id) {
            partner = &partners[i];
            break;
        }
    }

    if (!partner) {
        wprintf(L"Партнер не знайдений.\n");
        return;
    }

    wchar_t act_date[20];
    wprintf(L"Введіть дату складання акту (YYYY-MM-DD): ");
    fgetws(act_date, 20, stdin);
    act_date[wcslen(act_date) - 1] = L'\0';

    int act_id = get_next_act_id();

    wchar_t act_number[20];
    swprintf(act_number, 20, L"A-%03d", act_id);  // Формуємо номер акту

    _wmkdir(L"Acts");
    wchar_t filename[100];
    swprintf(filename, 100, L"Acts/Act_%ls.txt", act_number);

    FILE* file = _wfopen(filename, L"w, ccs=UTF-8");
    if (!file) {
        wprintf(L"Не вдалося створити файл акту.\n");
        return;
    }

    wchar_t first_initial = partner->first_name[0];
    wchar_t fathers_initial = partner->fathers_name[0];

    fwprintf(file,
        L"\t\t\t\tАКТ надання послуг №%ls від %ls\n\n"
        L"Ми, що нижче підписалися, представник Замовника %ls,\n"
        L"%ls %ls %ls %ls, з одного боку,\n"
        L"і представник Виконавця ТОВ \"ІНТАСК\", Директор Даценко Сергій Миколайович, з іншого боку,\n"
        L"склали цей акт про те, що на підставі наведених документів:\n\n"
        L"Договір: %ls № %ls від %ls\n"
        L"Замовлення №%d від %ls\n"
        L"Маршрут: %ls – %ls\n"
        L"Автомобіль: %ls\n\n"
        L"Надані послуги у повному обсязі:\n"
        L"Загальна вартість склала: %.2f грн, без ПДВ\n"
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
        act_number, act_date,
        partner->name, partner->position, partner->last_name, partner->first_name, partner->fathers_name,
        partner->details, partner->contract_number, partner->start_date,
        order->id, order->date_of_order,
        order->loading_place, order->unloading_place,
        order->vehicle_number,
        amount,
        partner->position, partner->last_name, first_initial, fathers_initial,
        partner->name,
        partner->tax_code,
        partner->address
    );

    fclose(file);

    Act new_act;
    new_act.id = act_id;
    wcscpy(new_act.act_number, act_number);
    wcscpy(new_act.date, act_date);
    new_act.order_id = order->id;
    new_act.partner_id = partner_id;
    wcscpy(new_act.act_type, act_type);

    FILE* bin_file = _wfopen(L"acts.dat", L"ab");
    if (!bin_file) {
        wprintf(L"Не вдалося відкрити файл acts.dat для запису.\n");
        return;
    }

    fwrite(&new_act, sizeof(Act), 1, bin_file);
    fclose(bin_file);

    wprintf(L"Акт успішно створено: %ls\n", filename);
}

int load_acts(Act* acts) {
    FILE* file = _wfopen(L"acts.dat", L"rb"); // без папки "data/"
    if (!file) {
        wprintf(L"Файл актів не знайдено. Повертаю порожній список.\n");
        return 0;
    }

    int count = 0;
    while (fread(&acts[count], sizeof(Act), 1, file) == 1 && count < MAX_ACTS) {
        count++;
    }

    fclose(file);
    return count;
}

void list_acts_period(
    Act* acts, int act_count,
    const wchar_t* from, const wchar_t* to
) {
    wprintf(L"\n=== Список актів (з %ls по %ls) ===\n", from, to);
    int found = 0;

    for (int i = 0; i < act_count; i++) {
        const wchar_t* d = acts[i].date;
        if (wcscmp(d, from) < 0 || wcscmp(d, to) > 0) {
            continue;
        }

        found = 1;
        wprintf(L"\nАкт ID: %d\n", acts[i].id);
        wprintf(L"  Номер акту: %ls\n", acts[i].act_number);
        wprintf(L"  Дата акту: %ls\n", acts[i].date);
        wprintf(L"  Замовлення ID: %d\n", acts[i].order_id);
        wprintf(L"  Партнер ID: %d\n", acts[i].partner_id);
        wprintf(L"  Тип акту: %ls\n", acts[i].act_type);
        wprintf(L"  ------------------------\n");
    }

    if (!found) {
        wprintf(L"⚠️ За вказаний період (%ls – %ls) актів не знайдено.\n", from, to);
    }
    wprintf(L"\n");
}

int get_next_payment_id() {
    // Спроба відкрити файл з останнім ID
    FILE* f = _wfopen(L"last_payment_id.txt", L"r, ccs=UNICODE");
    int last_id = 0;
    if (f) {
        fwscanf(f, L"%d", &last_id);
        fclose(f);
    }

    // Новий ID
    int next_id = last_id + 1;

    // Записуємо його назад
    f = _wfopen(L"last_payment_id.txt", L"w, ccs=UNICODE");
    if (f) {
        fwprintf(f, L"%d", next_id);
        fclose(f);
    }

    return next_id;
}

void save_payments() {
    FILE* f = _wfopen(L"data/payments.dat", L"wb");
    if (!f) return;
    fwrite(&payment_count, sizeof(int), 1, f);
    fwrite(payments, sizeof(Payment), payment_count, f);
    fclose(f);
}

int load_payments(Payment* payments) {
    FILE* file = _wfopen(L"payments.dat", L"rb");
    if (!file) {
        wprintf(L"Файл payments.dat не знайдено або не вдалося відкрити.\n");
        return 0;
    }

    int count = 0;
    // Читаємо доти, поки є повний запис Payment і не вичерпано масив
    while (count < MAX_PAYMENTS &&
        fread(&payments[count], sizeof(Payment), 1, file) == 1) {
        count++;
    }

    fclose(file);
    return count;
}

int generate_new_payment_id() {
    FILE* file = _wfopen(L"payments.txt", L"r, ccs=UNICODE");
    if (!file) return 1; // якщо файл не існує — ID починається з 1

    int max_id = 0;
    wchar_t line[512];
    while (fgetws(line, sizeof(line) / sizeof(wchar_t), file)) {
        int id;
        if (swscanf(line, L"{\"payment_id\": %d,", &id) == 1) {
            if (id > max_id) max_id = id;
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
    int order_id;
    wprintf(L"🔹 Введіть ID замовлення: ");
    wscanf(L"%d", &order_id);
    getchar(); // очищення буфера після wscanf

    Order orders[MAX_ORDERS];
    int order_count = load_orders(orders);
    if (order_count == 0) {
        wprintf(L"⚠️ Немає замовлень у системі.\n");
        return;
    }

    Order* order = find_order_by_id(order_id, orders, order_count);
    if (!order) {
        wprintf(L"❌ Замовлення з ID %d не знайдено.\n", order_id);
        return;
    }

    load_partners();
    if (partner_count == 0) {
        wprintf(L"⚠️ Немає партнерів у системі.\n");
        return;
    }

    Payment payment = { 0 };
    payment.payment_id = generate_new_payment_id();

    int type_choice;
    wprintf(L"🔹 Оберіть тип платежу:\n");
    wprintf(L"   [0] Від замовника (from_customer)\n");
    wprintf(L"   [1] Перевізнику (to_performer)\n");
    wprintf(L"Ваш вибір: ");
    wscanf(L"%d", &type_choice);
    getchar(); // очищення буфера

    if (type_choice == 0) {
        wcscpy(payment.payment_type, L"from_customer");
        payment.partner_id = order->customer_id;
        payment.amount_paid = order->freight;
    }
    else if (type_choice == 1) {
        wcscpy(payment.payment_type, L"to_performer");
        payment.partner_id = order->performer_id;
        payment.amount_paid = order->margin;
    }
    else {
        wprintf(L"❌ Невірний вибір типу платежу.\n");
        return;
    }

    payment.order_id = order_id;

    wprintf(L"🔹 Введіть дату платежу (у форматі YYYY-MM-DD): ");
    fgetws(payment.payment_date, 20, stdin);
    trim_newline(payment.payment_date);

    FILE* file = _wfopen(L"payments.txt", L"a, ccs=UTF-8");
    if (!file) {
        wprintf(L"❌ Не вдалося відкрити файл payments.txt для запису.\n");
        return;
    }

    fwprintf(file,
        L"{\"payment_id\": %d, \"payment_type\": \"%ls\", \"order_id\": %d, \"partner_id\": %d, \"amount_paid\": %.2f, \"payment_date\": \"%ls\"}\n",
        payment.payment_id, payment.payment_type, payment.order_id,
        payment.partner_id, payment.amount_paid, payment.payment_date);

    fclose(file);
    wprintf(L"✅ Платіж успішно створено та збережено.\n");
}

void list_payments_period(
    Payment* payments, int payment_count,
    const wchar_t* from, const wchar_t* to,
    Partner* partners, int partner_count
) {
    wprintf(L"\n=== Перелік платежів (з %ls по %ls) ===\n", from, to);
    int found = 0;

    for (int i = 0; i < payment_count; i++) {
        const wchar_t* d = payments[i].payment_date;
        if (wcscmp(d, from) < 0 || wcscmp(d, to) > 0) {
            continue;
        }
        found = 1;

        Partner* partner = find_partner_by_id(partners, partner_count, payments[i].partner_id);

        wprintf(L"\nПлатіж ID: %d\n", payments[i].payment_id);
        wprintf(L"  Тип: %ls\n", payments[i].payment_type);
        wprintf(L"  Замовлення ID: %d\n", payments[i].order_id);
        wprintf(L"  Партнер: %ls (ID %d)\n",
            partner ? partner->name : L"[ID не знайдено]",
            payments[i].partner_id);
        wprintf(L"  Сума: %.2lf грн\n", payments[i].amount_paid);
        wprintf(L"  Дата платежу: %ls\n", payments[i].payment_date);
        wprintf(L"  ------------------------\n");
    }

    if (!found) {
        wprintf(L"⚠️ За вказаний період (%ls – %ls) платежів не знайдено.\n", from, to);
    }
    wprintf(L"\n");
}

static void emit_invoice_for_order(const Order* o) {
    // Знайти клієнта й послугу
    Partner* customer = find_partner_by_id(partners, partner_count, o->customer_id);
    Service* service = find_service_by_id(services, service_count, o->service_id);

    // Згенерувати новий Invoice
    Invoice inv;
    inv.invoice_id = get_next_invoice_id();
    generate_invoice_number(inv.invoice_number, 20);
    inv.invoice_type = 0; // рахунок для замовника
    inv.order_id = o->id;
    inv.customer_id = o->customer_id;
    inv.service_id = o->service_id;
    wcscpy(inv.service_name, service ? service->service_name : L"");
    inv.amount_due = o->freight;
    wcscpy(inv.invoice_date, o->date_of_unloading);

    // Створити папку Invoices, якщо нема
    _wmkdir(L"Invoices");

    // Текстовий файл
    wchar_t filename[260];
    swprintf(filename, 260, L"Invoices\\%ls.txt", inv.invoice_number);
    FILE* f = _wfopen(filename, L"w, ccs=UTF-8");
    if (f) {
        fwprintf(f, L"ID рахунку: %d\n", inv.invoice_id);
        fwprintf(f, L"Рахунок на оплату № %ls від %ls\n\n\n",
            inv.invoice_number, inv.invoice_date);
        fwprintf(f, L"П/р UA343052990000026009006804903, Банк АТ КБ \"Приватбанк\", МФО 305299\n");
        fwprintf(f, L"08303, Київська обл., м. Бориспіль, вул. Нова, буд. 4, кв.72, тел.: 0673747000\n");
        fwprintf(f, L"код за ЄДРПОУ 37243792\nПлатник єдиного податку 3 група 5%%\n\n\n");
        fwprintf(f, L"Постачальник:\t\t\tТОВ \"ІНТАСК\"\n\n");
        fwprintf(f, L"Покупець:\t\t\t%ls\n", customer ? customer->name : L"[Невідомо]");
        fwprintf(f, L"Згідно договору: %ls, №%ls від %ls\n\n",
            customer ? customer->details : L"",
            customer ? customer->contract_number : L"",
            customer ? customer->start_date : L"");
        fwprintf(f, L"За %ls з %ls до %ls, а/м %ls\n",
            service ? service->service_name : L"",
            o->loading_place,
            o->unloading_place,
            o->vehicle_number);
        fwprintf(f, L"\nВсього найменувань 1, на суму %.2lf грн.\n\n\n\n\n",
            inv.amount_due);
        fwprintf(f, L"Рахунок виписав:\t\t\t Директор Даценко С.М.\n");
        fclose(f);
    }

    // Запис в двійковий файл
    FILE* df = _wfopen(L"invoices.dat", L"ab");
    if (df) {
        fwrite(&inv, sizeof(inv), 1, df);
        fclose(df);
    }
}

static void emit_act_for_order(const Order* o) {
    Partner* customer = find_partner_by_id(partners, partner_count, o->customer_id);
    Service* service = find_service_by_id(services, service_count, o->service_id);

    Act act;
    act.id = get_next_act_id();
    swprintf(act.act_number, 20, L"A-%05d", act.id);
    wcscpy(act.date, o->date_of_unloading);
    act.order_id = o->id;
    act.partner_id = o->customer_id;
    wcscpy(act.act_type, L"замовнику");

    _wmkdir(L"Acts");

    wchar_t filename[260];
    swprintf(filename, 260, L"Acts\\Act_%ls.txt", act.act_number);
    FILE* f = _wfopen(filename, L"w, ccs=UTF-8");
    if (f) {
        wchar_t fi = customer ? customer->first_name[0] : L'?';
        wchar_t fa = customer ? customer->fathers_name[0] : L'?';

        fwprintf(f,
            L"\t\t\t\tАКТ надання послуг №%ls від %ls\n\n"
            L"Ми, що нижче підписалися, представник Замовника %ls,\n"
            L"%ls %ls %ls %ls, з одного боку,\n"
            L"і представник Виконавця ТОВ \"ІНТАСК\", Директор Даценко Сергій Миколайович, з іншого боку,\n"
            L"склали цей акт про те, що на підставі наведених документів:\n\n"
            L"Договір: %ls № %ls від %ls\n"
            L"Замовлення №%d від %ls\n"
            L"Маршрут: %ls – %ls\n"
            L"Автомобіль: %ls\n\n"
            L"Надані послуги у повному обсязі:\n"
            L"Загальна вартість склала: %.2f грн, без ПДВ\n"
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
            act.act_number, act.date,
            customer ? customer->name : L"[Невідомо]",
            customer ? customer->position : L"",
            customer ? customer->last_name : L"", customer ? customer->first_name : L"", customer ? customer->fathers_name : L"",
            customer ? customer->details : L"", customer ? customer->contract_number : L"", customer ? customer->start_date : L"",
            o->id, o->date_of_order,
            o->loading_place, o->unloading_place,
            o->vehicle_number,
            (service ? o->freight : 0.0),
            customer ? customer->position : L"", customer ? customer->last_name : L"", fi, fa,
            customer ? customer->name : L"",
            customer ? customer->tax_code : L"",
            customer ? customer->address : L""
        );
        fclose(f);
    }

    // Запис в двійковий файл
    FILE* df = _wfopen(L"acts.dat", L"ab");
    if (df) {
        fwrite(&act, sizeof(act), 1, df);
        fclose(df);
    }
}

static void emit_payments_for_order(const Order* o) {
    // 1) Платіж від замовника
    Payment pay1;
    pay1.payment_id = get_next_payment_id();
    wcscpy(pay1.payment_type, L"from_customer");
    pay1.order_id = o->id;
    pay1.partner_id = o->customer_id;
    pay1.amount_paid = o->freight;
    wcscpy(pay1.payment_date, o->date_of_unloading);

    // 2) Платіж виконавцю
    Payment pay2;
    pay2.payment_id = get_next_payment_id();
    wcscpy(pay2.payment_type, L"to_performer");
    pay2.order_id = o->id;
    pay2.partner_id = o->performer_id;
    pay2.amount_paid = o->margin;
    wcscpy(pay2.payment_date, o->date_of_unloading);

    // Запис у payments.dat
    FILE* bf = _wfopen(L"payments.dat", L"ab");
    if (bf) {
        fwrite(&pay1, sizeof(pay1), 1, bf);
        fwrite(&pay2, sizeof(pay2), 1, bf);
        fclose(bf);
    }
}

bool has_invoice_for_order(int order_id, int invoice_type) {
    for (int i = 0; i < invoice_count; i++) {
        if (invoices[i].order_id == order_id
            && invoices[i].invoice_type == invoice_type)
            return true;
    }
    return false;
}

bool has_act_for_order(int order_id, const wchar_t* act_type) {
    for (int i = 0; i < act_count; i++) {
        if (acts[i].order_id == order_id
            && wcscmp(acts[i].act_type, act_type) == 0)
            return true;
    }
    return false;
}

bool has_payment_for_order(int order_id, const wchar_t* payment_type) {
    for (int i = 0; i < payment_count; i++) {
        if (payments[i].order_id == order_id
            && wcscmp(payments[i].payment_type, payment_type) == 0)
            return true;
    }
    return false;
}

void emit_payment_for_order(
    Order* o,
    const wchar_t* payment_type, // L"from_customer"/L"to_performer"
    const wchar_t* dat_file,     // L"payments.dat"
    Payment* payments,
    int* payment_count
) {
    Payment P;
    P.payment_id = get_next_payment_id();      // подібно до інших
    wcscpy(P.payment_type, payment_type);
    P.order_id = o->id;
    P.partner_id = wcscmp(payment_type, L"from_customer") == 0
        ? o->customer_id : o->performer_id;
    P.amount_paid = (wcscmp(payment_type, L"from_customer") == 0
        ? o->freight : o->margin);
    wcscpy(P.payment_date, o->date_of_unloading);

    FILE* dat = _wfopen(dat_file, L"a, ccs=UTF-8");
    fwprintf(dat,
        L"{\"payment_id\":%d,\"payment_type\":\"%ls\","
        L"\"order_id\":%d,\"partner_id\":%d,"
        L"\"amount_paid\":%.2f,\"payment_date\":\"%ls\"}\n",
        P.payment_id, P.payment_type,
        P.order_id, P.partner_id,
        P.amount_paid, P.payment_date
    );
    fclose(dat);

    payments[*payment_count] = P;
    (*payment_count)++;
}

void auto_generate_all_documents() {
    // 1) Завантажуємо наявні дані
    order_count = load_orders(orders);
    load_partners();
    load_services();
    invoice_count = load_invoices(invoices);
    act_count = load_acts(acts);
    payment_count = load_payments(payments);

    // 2) Створюємо всі необхідні папки
    _wmkdir(L"Invoices");
    _wmkdir(L"Acts");
    _wmkdir(L"InvoicesIn");
    _wmkdir(L"ActsIn");
    _wmkdir(L"PaymentsFromCustomer");
    _wmkdir(L"PaymentsToPerformer");

    int generated = 0;

    for (int i = 0; i < order_count; i++) {
        Order* o = &orders[i];

        // --- 1) Документи для замовника ---
        if (!has_invoice_for_order(o->id, 0)) {
            emit_invoice_for_order(o);
        }
        if (!has_act_for_order(o->id, L"замовнику")) {
            emit_act_for_order(o);
        }
        if (!has_payment_for_order(o->id, L"from_customer")) {
            // Генеруємо текстовий файл платежу
            Payment pay;
            pay.payment_id = get_next_payment_id();
            wcscpy(pay.payment_type, L"from_customer");
            pay.order_id = o->id;
            pay.partner_id = o->customer_id;
            pay.amount_paid = o->freight;
            wcscpy(pay.payment_date, o->date_of_unloading);

            // 1.1) Текстовий файл
            wchar_t fnameC[260];
            swprintf(fnameC, 260,
                L"PaymentsFromCustomer\\PAY_CUST%05d.txt",
                pay.payment_id);
            FILE* fC = _wfopen(fnameC, L"w, ccs=UTF-8");
            if (fC) {
                fwprintf(fC, L"Платіж ID: %d\n", pay.payment_id);
                fwprintf(fC, L"Тип: Від замовника\n");
                fwprintf(fC, L"Замовлення ID: %d\n", pay.order_id);
                fwprintf(fC, L"Сума: %.2f грн\n", pay.amount_paid);
                fwprintf(fC, L"Дата: %ls\n", pay.payment_date);
                fclose(fC);
            }

            // 1.2) Двійковий запис
            FILE* bf1 = _wfopen(L"payments.dat", L"ab");
            fwrite(&pay, sizeof pay, 1, bf1);
            fclose(bf1);
            payments[payment_count++] = pay;
        }

        // --- 2) Документи для перевізника ---
        if (!has_invoice_for_order(o->id, 1)) {
            // (аналогічно вхідному рахунку)
            Partner* perf = find_partner_by_id(partners, partner_count, o->performer_id);
            Service* serv = find_service_by_id(services, service_count, o->service_id);

            Invoice inv;
            inv.invoice_id = get_next_invoice_id();
            swprintf(inv.invoice_number, 20, L"INV_IN%05d", inv.invoice_id);
            inv.invoice_type = 1;
            inv.order_id = o->id;
            inv.customer_id = o->performer_id;
            inv.service_id = o->service_id;
            wcscpy(inv.service_name, serv ? serv->service_name : L"");
            inv.amount_due = o->margin;
            wcscpy(inv.invoice_date, o->date_of_unloading);

            wchar_t fnI[260];
            swprintf(fnI, 260, L"InvoicesIn\\%ls.txt", inv.invoice_number);
            FILE* fI = _wfopen(fnI, L"w, ccs=UTF-8");
            if (fI) {
                fwprintf(fI, L"ID вхідного рахунку: %d\n", inv.invoice_id);
                fwprintf(fI, L"Рахунок вхідний № %ls від %ls\n", inv.invoice_number, inv.invoice_date);
                fwprintf(fI, L"Перевізник: %ls\n", perf ? perf->name : L"[Н/Д]");
                fwprintf(fI, L"Сума: %.2f грн\n", inv.amount_due);
                fclose(fI);
            }
            FILE* dI = _wfopen(L"invoices.dat", L"ab");
            fwrite(&inv, sizeof inv, 1, dI);
            fclose(dI);
            invoices[invoice_count++] = inv;
        }
        if (!has_act_for_order(o->id, L"перевізнику")) {
            Partner* perf = find_partner_by_id(partners, partner_count, o->performer_id);
            Act act;
            act.id = get_next_act_id();
            swprintf(act.act_number, 20, L"ACT_IN%05d", act.id);
            wcscpy(act.date, o->date_of_unloading);
            act.order_id = o->id;
            act.partner_id = o->performer_id;
            wcscpy(act.act_type, L"перевізнику");

            wchar_t fnA[260];
            swprintf(fnA, 260, L"ActsIn\\%ls.txt", act.act_number);
            FILE* fA = _wfopen(fnA, L"w, ccs=UTF-8");
            if (fA) {
                fwprintf(fA, L"Акт вхідний № %ls від %ls\n", act.act_number, act.date);
                fwprintf(fA, L"Перевізник: %ls\n", perf ? perf->name : L"[Н/Д]");
                fwprintf(fA, L"Сума: %.2f грн\n", o->margin);
                fclose(fA);
            }
            FILE* dA = _wfopen(L"acts.dat", L"ab");
            fwrite(&act, sizeof act, 1, dA);
            fclose(dA);
            acts[act_count++] = act;
        }
        if (!has_payment_for_order(o->id, L"to_performer")) {
            Payment pay;
            pay.payment_id = get_next_payment_id();
            wcscpy(pay.payment_type, L"to_performer");
            pay.order_id = o->id;
            pay.partner_id = o->performer_id;
            pay.amount_paid = o->margin;
            wcscpy(pay.payment_date, o->date_of_unloading);

            // Текстовий файл
            wchar_t fnP[260];
            swprintf(fnP, 260,
                L"PaymentsToPerformer\\PAY_PER%05d.txt",
                pay.payment_id);
            FILE* fP = _wfopen(fnP, L"w, ccs=UTF-8");
            if (fP) {
                fwprintf(fP, L"Платіж ID: %d\n", pay.payment_id);
                fwprintf(fP, L"Тип: До перевізника\n");
                fwprintf(fP, L"Замовлення ID: %d\n", pay.order_id);
                fwprintf(fP, L"Сума: %.2f грн\n", pay.amount_paid);
                fwprintf(fP, L"Дата: %ls\n", pay.payment_date);
                fclose(fP);
            }

            // Двійковий запис
            FILE* bf2 = _wfopen(L"payments.dat", L"ab");
            fwrite(&pay, sizeof pay, 1, bf2);
            fclose(bf2);
            payments[payment_count++] = pay;
        }

        generated++;
    }

    wprintf(L"✅ Згенеровано документи для %d замовлень.\n", generated);
}

// --- Звіти по рейсах ----

void list_orders_table(Order* orders, int order_count,
    Partner* partners, int partner_count,
    Service* services, int service_count) {
    if (order_count == 0) {
        wprintf(L"Немає замовлень для виводу.\n");
        return;
    }

    wprintf(L"\nСписок усіх замовлень:\n");
    wprintf(L"-----------------------------------------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-3s | %-12s | %-16s | %-14s | %-18s | %-9s | %-9s | %-19s | %-10s |\n",
        L"ID", L"Замовник", L"Виконавець",
        L"Завантаження", L"Розвантаження",
        L"Вартість", L"Ціна",
        L"Авто", L"Дата розв.");
    wprintf(L"-----------------------------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < order_count; i++) {
        Partner* customer = find_partner_by_id(partners, partner_count, orders[i].customer_id);
        Partner* performer = find_partner_by_id(partners, partner_count, orders[i].performer_id);

        wprintf(L"| %-3d | %-12ls | %-16ls | %-14ls | %-18ls | %9.2f | %9.2f | %-19ls | %-10ls |\n",
            orders[i].id,
            customer ? customer->name : L"[Невідомо]",
            performer ? performer->name : L"[Невідомо]",
            orders[i].loading_place,
            orders[i].unloading_place,
            orders[i].freight,
            orders[i].margin,
            orders[i].vehicle_number,
            orders[i].date_of_unloading
        );
    }

    wprintf(L"---------------------------------------------------------------------------------------------------------------------------------------\n\n");
}

void list_order_summary(
    Order* orders, int order_count,
    Invoice* invoices, int invoice_count,
    Act* acts, int act_count
) {
    if (order_count == 0) {
        wprintf(L"Немає замовлень для виводу зведеної інформації.\n");
        return;
    }

    wchar_t date_from[11], date_to[11];

    wprintf(L"Введіть початок періоду (YYYY-MM-DD): ");
    wscanf(L"%10ls", date_from);
    clear_input_buffer();  // скасувати залишок після wscanf

    wprintf(L"Введіть кінець періоду   (YYYY-MM-DD): ");
    wscanf(L"%10ls", date_to);
    clear_input_buffer();

    // 2) Заголовок
    wprintf(L"\n=== Зведена таблиця по замовленнях (%ls – %ls) ===\n",
        date_from, date_to);
    wprintf(L"-----------------------------------------------------------------------------------------------------\n");
    wprintf(L"| ID | № рах. (зам.)  | № акту (зам.) | Дохід, грн   | Витрати, грн | Прибуток, грн | Маржинальність, %% |\n");
    wprintf(L"-----------------------------------------------------------------------------------------------------\n");

    // 3) Ітерація та підсумки
    double total_rev = 0.0,
        total_cost = 0.0,
        total_prof = 0.0,
        sum_mperc = 0.0;
    int    count_rows = 0;

    for (int i = 0; i < order_count; i++) {
        const wchar_t* unload_date = orders[i].date_of_unloading;
        // Фільтр: date_from <= unload_date <= date_to
        if (wcscmp(unload_date, date_from) < 0 ||
            wcscmp(unload_date, date_to) > 0)
            continue;

        int   oid = orders[i].id;
        double revenue = 0.0;
        const wchar_t* inv_num = L"-";
        Act* act_c = NULL;

        // Знаходимо рахунок та акт "замовнику"
        for (int j = 0; j < invoice_count; j++) {
            if (invoices[j].order_id == oid &&
                invoices[j].invoice_type == INVOICE_FROM_CUSTOMER) {
                revenue = invoices[j].amount_due;
                inv_num = invoices[j].invoice_number;
                break;
            }
        }
        for (int k = 0; k < act_count; k++) {
            if (acts[k].order_id == oid &&
                wcscmp(acts[k].act_type, L"замовнику") == 0) {
                act_c = &acts[k];
                break;
            }
        }

        double cost = orders[i].margin;
        double profit = revenue - cost;
        double mperc = revenue > 0.0
            ? (profit / revenue) * 100.0
            : 0.0;

        // Друкуємо рядок
        wprintf(L"| %-2d | %-14ls | %-13ls | %12.2f | %12.2f | %13.2f | %13.2f %% |\n",
            oid,
            inv_num,
            act_c ? act_c->act_number : L"-",
            revenue,
            cost,
            profit,
            mperc
        );

        // Нараховуємо підсумки
        total_rev += revenue;
        total_cost += cost;
        total_prof += profit;
        sum_mperc += mperc;
        count_rows++;
    }

    // Якщо немає записів за період
    if (count_rows == 0) {
        wprintf(L"-----------------------------------------------------------------------------------------------------\n");
        wprintf(L"⚠️ В період з %ls по %ls не знайдено жодного замовлення.\n\n",
            date_from, date_to);
        return;
    }

    // 4) Підсумковий рядок
    double avg_mperc = sum_mperc / count_rows;
    wprintf(L"------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-31s | %12.2f | %12.2f | %13.2f | %17.2f %% |\n",
        L"Всього:",
        total_rev,
        total_cost,
        total_prof,
        avg_mperc
    );
    wprintf(L"------------------------------------------------------------------------------------------------------\n\n");
}

void show_top_margin_orders() {
    int n;
    wprintf(L"Скільки рейсів показати? ");
    wscanf(L"%d", &n);
    clear_input_buffer(); // очищення буфера після введення

    // Завантаження
    order_count = load_orders(orders);
    load_partners(); // Потрібен partner_count і partners[]

    OrderWithMargin temp[MAX_ORDERS];

    // Заповнення масиву з margin_value
    for (int i = 0; i < order_count; i++) {
        double margin_value = ((orders[i].freight - orders[i].margin) * 100.0) / orders[i].freight;
        temp[i].order = orders[i];
        temp[i].margin_value = margin_value;
    }

    // Сортування за спаданням margin_value (bubble sort для простоти)
    for (int i = 0; i < order_count - 1; i++) {
        for (int j = i + 1; j < order_count; j++) {
            if (temp[i].margin_value < temp[j].margin_value) {
                OrderWithMargin t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    wprintf(L"\n%-5s %-30s %-20s %-20s %-15s %-10s\n",
        L"ID", L"Замовник", L"Завантаження", L"Розвантаження", L"Дата", L"Рентабельність рейсу %");

    for (int i = 0; i < n && i < order_count; i++) {
        Order o = temp[i].order;
        Partner* customer = find_partner_by_id(partners, partner_count, o.customer_id);

        wprintf(L"%-5d %-30s %-20s %-20s %-15s %-10.2f\n",
            o.id,
            customer ? customer->name : L"(Невідомий)",
            o.loading_place,
            o.unloading_place,
            o.date_of_unloading,
            temp[i].margin_value);
    }
}

static int cmp_row(const void* a, const void* b) {
    const Row* x = (const Row*)a;
    const Row* y = (const Row*)b;
    return x->partner_id - y->partner_id;
}

static void _report_aging_grouped(
    const wchar_t* title,
    Order* orders, int order_count,
    Act* acts, int act_count,
    Payment* payments, int payment_count,
    Partner* partners, int partner_count,
    int is_debtor // 1 = дебіторська, 0 = кредиторська
) {
    const wchar_t* act_type = is_debtor ? L"замовнику" : L"перевізнику";

    typedef struct {
        int partner_id;
        int order_id;
        double dt;
        double kt;
    } Row;

    Row* rows = (Row*)malloc(sizeof(Row) * order_count);
    if (!rows) {
        wprintf(L"❌ Помилка виділення пам'яті.\n");
        return;
    }

    int rcount = 0;

    for (int i = 0; i < order_count; i++) {
        Order* o = &orders[i];

        // Має бути акт відповідного типу
        int found_act = 0;
        for (int a = 0; a < act_count; a++) {
            if (acts[a].order_id == o->id &&
                wcscmp(acts[a].act_type, act_type) == 0) {
                found_act = 1;
                break;
            }
        }
        if (!found_act) continue;

        rows[rcount].partner_id = is_debtor ? o->customer_id : o->performer_id;
        rows[rcount].order_id = o->id;

        if (is_debtor) {
            rows[rcount].dt = o->freight;
            rows[rcount].kt = o->freight;
        }
        else {
            rows[rcount].dt = o->margin;
            rows[rcount].kt = o->margin;
        }

        rcount++;
    }

    wprintf(L"\n=== %ls ===\n", title);
    if (rcount == 0) {
        wprintf(L"⚠️ Жодного запису.\n\n");
        free(rows);
        return;
    }

    qsort(rows, rcount, sizeof(Row), cmp_row);

    wprintf(L"----------------------------------------------------------------------------\n");
    wprintf(L"| %4s | %-20s | %12s | %12s |\n",
        L"ID",
        is_debtor ? L"Замовник" : L"Перевізник",
        L"DT", L"KT");
    wprintf(L"----------------------------------------------------------------------------\n");

    double grand_dt = 0.0, grand_kt = 0.0;
    int cur_pid = rows[0].partner_id;
    double sub_dt = 0.0, sub_kt = 0.0;

    for (int i = 0; i < rcount; i++) {
        Row* R = &rows[i];

        if (R->partner_id != cur_pid) {
            Partner* P = find_partner_by_id(partners, partner_count, cur_pid);
            wprintf(L"| %4s | %-20ls | %12.2f | %12.2f |\n",
                L"", P ? P->name : L"[невідомо]",
                sub_dt, sub_kt);
            wprintf(L"----------------------------------------------------------------------------\n");
            sub_dt = sub_kt = 0.0;
            cur_pid = R->partner_id;
        }

        Partner* P = find_partner_by_id(partners, partner_count, R->partner_id);
        wprintf(L"| %4d | %-20ls | %12.2f | %12.2f |\n",
            R->order_id,
            P ? P->name : L"[невідомо]",
            R->dt, R->kt);

        sub_dt += R->dt;
        sub_kt += R->kt;
        grand_dt += R->dt;
        grand_kt += R->kt;
    }

    Partner* Plast = find_partner_by_id(partners, partner_count, cur_pid);
    wprintf(L"| %4s | %-20ls | %12.2f | %12.2f |\n",
        L"", Plast ? Plast->name : L"[невідомо]",
        sub_dt, sub_kt);
    wprintf(L"----------------------------------------------------------------------------\n");

    wprintf(L"| %4s | %-20s | %12.2f | %12.2f |\n",
        L"", L"Всього:", grand_dt, grand_kt);
    wprintf(L"----------------------------------------------------------------------------\n\n");

    free(rows);
}

void report_debtor_aging(
    Order* orders, int order_count,
    Act* acts, int act_count,
    Payment* payments, int payment_count,
    Partner* partners, int partner_count
) {
    _report_aging_grouped(
        L"Аналіз дебіторської заборгованості",
        orders, order_count,
        acts, act_count,
        payments, payment_count,
        partners, partner_count,
        1
    );
}

void report_creditor_aging(
    Order* orders, int order_count,
    Act* acts, int act_count,
    Payment* payments, int payment_count,
    Partner* partners, int partner_count
) {
    _report_aging_grouped(
        L"Аналіз кредиторської заборгованості",
        orders, order_count,
        acts, act_count,
        payments, payment_count,
        partners, partner_count,
        0
    );
}


//  --- Заробітна плата ---

void load_employees() {
    FILE* f = _wfopen(L"employees.dat", L"rb");
    if (!f) {
        // якщо файл ще не створений — просто нульова кількість
        employee_count = 0;
        return;
    }
    // Спочатку читаємо число співробітників
    fread(&employee_count, sizeof(int), 1, f);
    // Потім — масив Employee
    fread(employees, sizeof(Employee), employee_count, f);
    fclose(f);
}

void save_employees() {
    FILE* file = _wfopen(L"employees.dat", L"wb");
    if (!file) return;
    fwrite(&employee_count, sizeof(int), 1, file);
    fwrite(employees, sizeof(Employee), employee_count, file);
    fclose(file);
}

void add_employee() {
    if (employee_count >= MAX_EMPLOYEES) {
        wprintf(L"⚠️ Досягнуто максимальну кількість співробітників (%d).\n", MAX_EMPLOYEES);
        return;
    }

    // Очищуємо буфер перед читанням рядків
    clear_input_buffer();

    Employee e;
    e.id = (employee_count == 0) ? 1 : (employees[employee_count - 1].id + 1);

    wprintf(L"Прізвище: ");
    fgetws(e.last_name, sizeof(e.last_name) / sizeof(wchar_t), stdin);
    trim_newline(e.last_name);

    wprintf(L"Ім’я: ");
    fgetws(e.first_name, sizeof(e.first_name) / sizeof(wchar_t), stdin);
    trim_newline(e.first_name);

    wprintf(L"По батькові: ");
    fgetws(e.fathers_name, sizeof(e.fathers_name) / sizeof(wchar_t), stdin);
    trim_newline(e.fathers_name);

    wprintf(L"Стать: ");
    fgetws(e.gender, sizeof(e.gender) / sizeof(wchar_t), stdin);
    trim_newline(e.gender);

    wprintf(L"Податковий номер: ");
    fgetws(e.tax_number, sizeof(e.tax_number) / sizeof(wchar_t), stdin);
    trim_newline(e.tax_number);

    wprintf(L"Посада: ");
    fgetws(e.position, sizeof(e.position) / sizeof(wchar_t), stdin);
    trim_newline(e.position);

    // Додаємо до масиву
    employees[employee_count++] = e;

    // Зберігаємо всіх співробітників у файл
    FILE* f = _wfopen(L"employees.dat", L"wb");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити employees.dat для запису.\n");
        return;
    }
    // Спочатку збережемо кількість
    fwrite(&employee_count, sizeof(int), 1, f);
    // Потім — масив структур
    fwrite(employees, sizeof(Employee), employee_count, f);
    fclose(f);

    wprintf(L"✅ Співробітника збережено з ID=%d.\n", e.id);
}

void list_employees() {
    load_employees();  // <-- Ось тут!

    if (employee_count == 0) {
        wprintf(L"⚠️ Немає збережених співробітників.\n");
        return;
    }

    wprintf(L"\n%-5s %-15s %-15s %-15s %-10s %-15s %-20s\n",
        L"ID", L"Прізвище", L"Ім’я", L"По батькові", L"Стать", L"ІПН", L"Посада");
    for (int i = 0; i < employee_count; i++) {
        Employee e = employees[i];
        wprintf(L"%-5d %-15ls %-15ls %-15ls %-10ls %-15ls %-20ls\n",
            e.id, e.last_name, e.first_name, e.fathers_name, e.gender, e.tax_number, e.position);
    }
}


void calculate_salary() {
    int id;
    double gross_salary;
    wchar_t month[32];

    // 1) Вводимо ID
    wprintf(L"Введіть ID співробітника: ");
    wscanf(L"%d", &id);
    clear_input_buffer();  // <-- Очищуємо буфер

    // Шукаємо співробітника
    Employee* emp = NULL;
    for (int i = 0; i < employee_count; i++) {
        if (employees[i].id == id) {
            emp = &employees[i];
            break;
        }
    }
    if (!emp) {
        wprintf(L"❌ Співробітника з ID %d не знайдено.\n", id);
        return;
    }

    // 2) Вводимо місяць
    wprintf(L"Введіть місяць, за який нараховується ЗП (наприклад, Травень 2025): ");
    fgetws(month, sizeof(month) / sizeof(wchar_t), stdin);
    // Видаляємо кінцевий '\n'
    size_t len = wcslen(month);
    if (len > 0 && month[len - 1] == L'\n') month[len - 1] = L'\0';

    // 3) Вводимо нараховану ЗП
    wprintf(L"Введіть нараховану заробітну плату: ");
    wscanf(L"%lf", &gross_salary);
    clear_input_buffer();  // <-- Очищуємо буфер

    // 4) Розрахунки
    double military_tax = gross_salary * 0.05;   // 5%
    double income_tax = gross_salary * 0.18;   // 18%
    double social_contrib = gross_salary * 0.22;   // 22% (сплачує роботодавець)
    double net_salary = gross_salary - military_tax - income_tax;

    // 5) Створюємо папку, якщо нема
    _wmkdir(L"Payrolls");

    // 6) Формуємо ім’я файлу: payroll_ID.txt
    wchar_t filename[128];
    swprintf(filename, 128, L"Payrolls/payroll_%d.txt", emp->id);

    FILE* f = _wfopen(filename, L"a, ccs=UTF-8");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити файл %ls для запису.\n", filename);
        return;
    }

    // 7) Записуємо відомість
    fwprintf(f, L"=== Розрахунковий листок ===\n");
    fwprintf(f, L"Місяць: %ls\n", month);
    fwprintf(f, L"ПІБ: %ls %ls %ls\n", emp->last_name, emp->first_name, emp->fathers_name);
    fwprintf(f, L"Посада: %ls\n", emp->position);
    fwprintf(f, L"Податковий номер: %ls\n", emp->tax_number);
    fwprintf(f, L"Нараховано: %.2f грн\n", gross_salary);
    fwprintf(f, L"Військовий збір (5%%): %.2f грн\n", military_tax);
    fwprintf(f, L"ПДФО (18%%): %.2f грн\n", income_tax);
    fwprintf(f, L"ЄСВ (22%%, роботодавець): %.2f грн\n", social_contrib);
    fwprintf(f, L"Сума до видачі: %.2f грн\n", net_salary);
    fwprintf(f, L"===========================\n\n");

    fclose(f);

    wprintf(L"✅ Відомість збережено у файл: %ls\n", filename);
}

void view_payrolls() {
    struct _wfinddata_t fileinfo;
    intptr_t handle;

    // Шукаємо всі .txt у папці Payrolls
    handle = _wfindfirst(L"Payrolls\\*.txt", &fileinfo);
    if (handle == -1) {
        wprintf(L"ℹ️ Папка Payrolls порожня або не існує.\n");
        return;
    }

    do {
        // Повний шлях до файлу
        wchar_t filepath[260];
        swprintf(filepath, 260, L"Payrolls\\%ls", fileinfo.name);

        wprintf(L"\n--- %ls ---\n", fileinfo.name);

        FILE* f = _wfopen(filepath, L"r, ccs=UTF-8");
        if (!f) {
            wprintf(L"❌ Не вдалося відкрити файл %ls\n", fileinfo.name);
            continue;
        }

        // Виводимо вміст
        wchar_t line[256];
        while (fgetws(line, sizeof(line) / sizeof(wchar_t), f)) {
            wprintf(L"%ls", line);
        }
        fclose(f);
    } while (_wfindnext(handle, &fileinfo) == 0);

    _findclose(handle);
}

void edit_employee() {
    load_employees();  // <-- А тут теж

    if (employee_count == 0) {
        wprintf(L"⚠️ Немає співробітників для редагування.\n");
        return;
    }

    int id;
    wprintf(L"Введіть ID співробітника для редагування: ");
    wscanf(L"%d", &id);
    clear_input_buffer();

    // Шукаємо співробітника
    Employee* e = NULL;
    for (int i = 0; i < employee_count; i++) {
        if (employees[i].id == id) {
            e = &employees[i];
            break;
        }
    }
    if (!e) {
        wprintf(L"❌ Співробітника з ID %d не знайдено.\n", id);
        return;
    }

    int choice;
    do {
        // Виводимо поточні значення
        wprintf(L"\nРедагування співробітника ID=%d:\n", e->id);
        wprintf(L"1) Прізвище:        %ls\n", e->last_name);
        wprintf(L"2) Ім’я:            %ls\n", e->first_name);
        wprintf(L"3) По батькові:     %ls\n", e->fathers_name);
        wprintf(L"4) Стать:           %ls\n", e->gender);
        wprintf(L"5) Податковий номер:%ls\n", e->tax_number);
        wprintf(L"6) Посада:          %ls\n", e->position);
        wprintf(L"0) Зберегти та вийти\n");
        wprintf(L"Ваш вибір: ");
        wscanf(L"%d", &choice);
        clear_input_buffer();

        wchar_t buffer[64];
        switch (choice) {
        case 1:
            wprintf(L"Введіть нове прізвище: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->last_name, buffer);
            break;
        case 2:
            wprintf(L"Введіть нове ім’я: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->first_name, buffer);
            break;
        case 3:
            wprintf(L"Введіть нове по батькові: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->fathers_name, buffer);
            break;
        case 4:
            wprintf(L"Введіть нову стать: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->gender, buffer);
            break;
        case 5:
            wprintf(L"Введіть новий податковий номер: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->tax_number, buffer);
            break;
        case 6:
            wprintf(L"Введіть нову посаду: ");
            fgetws(buffer, 64, stdin); trim_newline(buffer);
            wcscpy(e->position, buffer);
            break;
        case 0:
            break;
        default:
            wprintf(L"Невірний вибір. Спробуйте ще раз.\n");
        }
    } while (choice != 0);

    // Після редагування — зберігаємо всі співробітники
    FILE* f = _wfopen(L"employees.dat", L"wb");
    if (!f) {
        wprintf(L"❌ Не вдалося відкрити файл для збереження.\n");
        return;
    }
    fwrite(&employee_count, sizeof(int), 1, f);
    fwrite(employees, sizeof(Employee), employee_count, f);
    fclose(f);

    wprintf(L"✅ Зміни збережено.\n");
}

// --- Податкові звіти ---

static bool wcsistr(const wchar_t* hay, const wchar_t* needle) {
    size_t hlen = wcslen(hay), nlen = wcslen(needle);
    for (size_t i = 0; i + nlen <= hlen; i++) {
        if (_wcsnicmp(hay + i, needle, nlen) == 0) return true;
    }
    return false;
}

void report_esv(const wchar_t* period) {
    load_employees();

    wprintf(L"\n=== Звіт по ЄСВ за %ls ===\n", period);
    wprintf(L"------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-30s | %-15s | %15s | %10s |\n",
        L"ID", L"ПІБ", L"ІПН", L"Нараховано ЗП", L"ЄСВ (22%)");
    wprintf(L"------------------------------------------------------------\n");

    double total_gross = 0, total_esv = 0;
    for (int i = 0; i < employee_count; i++) {
        Employee* e = &employees[i];
        wchar_t fname[100];
        swprintf(fname, 100, L"Payrolls/payroll_%d.txt", e->id);
        FILE* f = _wfopen(fname, L"r, ccs=UTF-8");
        if (!f) continue;

        wchar_t line[256];
        double gross = 0;
        bool in_period = false;
        // читаємо блок за період, беремо рядок "Нараховано: ..."
        while (fgetws(line, 256, f)) {
            if (!in_period && wcsistr(line, period)) {
                in_period = true;
                continue;
            }
            if (in_period && wcsstr(line, L"Нараховано:")) {
                swscanf(line, L"Нараховано: %lf", &gross);
                break;
            }
        }
        fclose(f);

        if (in_period && gross > 0) {
            double esv = gross * 0.22;
            total_gross += gross;
            total_esv += esv;

            wchar_t fio[100];
            swprintf(fio, 100, L"%ls %ls %ls", e->last_name, e->first_name, e->fathers_name);
            wprintf(L"| %-5d | %-30ls | %-15ls | %15.2f | %10.2f |\n",
                e->id, fio, e->tax_number, gross, esv);
        }
    }

    wprintf(L"------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-30s | %-15s | %15.2f | %10.2f |\n",
        L"", L"Всього по всім", L"", total_gross, total_esv);
    wprintf(L"------------------------------------------------------------\n\n");
}

void report_1df(const wchar_t* period) {
    load_employees();

    wprintf(L"\n=== Звіт 1ДФ за %ls ===\n", period);
    wprintf(L"-------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-30s | %-15s | %-10s | %-10s |\n",
        L"ID", L"ПІБ", L"ІПН", L"ВЗ (5%)", L"ПДФО (18%)");
    wprintf(L"-------------------------------------------------------------\n");

    double total_vz = 0, total_pdo = 0;
    for (int i = 0; i < employee_count; i++) {
        Employee* e = &employees[i];
        wchar_t fname[100];
        swprintf(fname, 100, L"Payrolls/payroll_%d.txt", e->id);
        FILE* f = _wfopen(fname, L"r, ccs=UTF-8");
        if (!f) continue;

        wchar_t line[256];
        double vz = 0, pdo = 0;
        bool in_period = false;
        while (fgetws(line, 256, f)) {
            if (!in_period && wcsistr(line, period)) {
                in_period = true;
                continue;
            }
            if (in_period) {
                if (wcsstr(line, L"Військовий збір")) {
                    swscanf(line, L"Військовий збір (%*d%%): %lf", &vz);
                }
                else if (wcsstr(line, L"ПДФО")) {
                    swscanf(line, L"ПДФО (%*d%%): %lf", &pdo);
                    break;
                }
            }
        }
        fclose(f);

        if (in_period) {
            total_vz += vz;
            total_pdo += pdo;
            wchar_t fio[100];
            swprintf(fio, 100, L"%ls %ls %ls", e->last_name, e->first_name, e->fathers_name);
            wprintf(L"| %-5d | %-30ls | %-15ls | %10.2f | %10.2f |\n",
                e->id, fio, e->tax_number, vz, pdo);
        }
    }

    wprintf(L"-------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-30s | %-15s | %10.2f | %10.2f |\n",
        L"", L"Всього по всім", L"", total_vz, total_pdo);
    wprintf(L"-------------------------------------------------------------\n\n");
}


static bool date_in_range(const wchar_t* date, const wchar_t* from, const wchar_t* to) {
    return (wcscmp(date, from) >= 0) && (wcscmp(date, to) <= 0);
}

void report_corporate_tax(const wchar_t* from_date, const wchar_t* to_date) {
    order_count = load_orders(orders);

    wprintf(L"\n=== Дані для єдиного податку (з %ls до %ls) ===\n", from_date, to_date);
    wprintf(L"--------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-15s | %10s | %15s |\n",
        L"ID", L"Дата замовлення", L"Прибуток", L"Єдиний податок (5%)");
    wprintf(L"--------------------------------------------------------------\n");

    double total_profit = 0, total_tax = 0;
    for (int i = 0; i < order_count; i++) {
        Order* o = &orders[i];
        if (date_in_range(o->date_of_order, from_date, to_date)) {
            double profit = o->freight - o->margin;
            double single_tax = profit * 0.05;
            total_profit += profit;
            total_tax += single_tax;
            wprintf(L"| %-5d | %-15ls | %10.2f | %15.2f |\n",
                o->id, o->date_of_order, profit, single_tax);
        }
    }

    wprintf(L"--------------------------------------------------------------\n");
    wprintf(L"| %-5s | %-15s | %10.2f | %15.2f |\n",
        L"", L"Всього", total_profit, total_tax);
    wprintf(L"--------------------------------------------------------------\n\n");
}




void run_menu() {
    wchar_t input[10];
    int choice = -1;

    do {
        wprintf(L"\n\t\t\t\t\t=== Головне меню ===\n");
        wprintf(L"\n--- Довідники ----\t\t\t--- Замовлення ---\t\t\t--- Фінансові документи ---\n\n");
        wprintf(L"1. Додати партнера\t\t\t6. Додати замовлення\t\t\t10. Сформувати рахунок\n");
        wprintf(L"2. Переглянути партнерів\t\t7. Переглянути замовлення\t\t11. Переглянути рахунки\n");
        wprintf(L"3. Видалити партнера\t\t\t8. Видалити замовлення\t\t\t12. Створити акт\n");
        wprintf(L"4. Додати послугу\t\t\t9. Редагувати замовлення\t\t13. Переглянути акти\n");
        wprintf(L"5. Переглянути послуги\t\t\t27. Завантажити із CSV\t\t\t14. Додати платіж\n");
        wprintf(L"28. Редагувати партнера\t\t\t\t\t\t\t\t15. Перегляд платежів\n");
        wprintf(L"29. Видалити послугу\t\t\t\t\t\t\t\t99. Згенерувати всі документи\n");
        wprintf(L"30. Редагувати послугу\n");
        wprintf(L"\n--- Звіти по рейсах ----\t\t--- Заробітна плата ---\t\t\t--- Податкові звіти ---\n\n");
        wprintf(L"16. Вивести журнал рейсів\t\t19. Додати співробітника\t\t24. Розрахунок ЄСВ\n");
        wprintf(L"17. Розрахувати маржинальність\t\t20. Переглянути співробітників\t\t25. Розрахунок ПДФО та ВЗ\n");
        wprintf(L"18. Показати рентабельність\t\t21. Розрахувати заробітну плату\t\t26. Розрахунок єдиного податку\n");
        wprintf(L"31. Дебіторська заборгованість\t\t22. Переглянути розрахункові листки\n");
        wprintf(L"32. Кредиторська заборгованість\t\t23. Редагувати співробітника\n\n");
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
        case 6:
            order_count = load_orders(orders);
            add_order();
            order_count = load_orders(orders);
            break;
        case 7: {
            // 1) Зчитуємо межі періоду
            wchar_t from[20], to[20];
            wprintf(L"Введіть початок періоду (YYYY-MM-DD): ");
            fgetws(from, 20, stdin);
            trim_newline(from);

            wprintf(L"Введіть кінець періоду (YYYY-MM-DD): ");
            fgetws(to, 20, stdin);
            trim_newline(to);

            // 2) Завантажуємо довідники (щоби заповнити partners[] та services[])
            load_partners();    // встановить global partner_count
            load_services();    // встановить global service_count

            // 3) Завантажуємо масив замовлень
            order_count = load_orders(orders);

            // 4) Викликаємо функцію з періодом
            list_orders(
                orders,         // масив Order
                order_count,    // кількість завантажених замовлень
                partners,       // масив Partner
                partner_count,  // кількість завантажених партнерів
                services,       // масив Service
                service_count,  // кількість завантажених послуг
                from,           // початок періоду "YYYY-MM-DD"
                to              // кінець періоду "YYYY-MM-DD"
            );

            break;
        }

        case 8: {
            order_count = load_orders(orders);
            wprintf(L"Введіть ID замовлення для видалення: ");
            int del_id;
            wscanf(L"%d", &del_id);
            clear_input_buffer();
            delete_order_by_id(del_id);
            order_count = load_orders(orders);
            break;
        }
        case 9:
            order_count = load_orders(orders);
            edit_order();
            order_count = load_orders(orders);
            break;
        case 10: create_invoice(); break;
        case 11: {
            wchar_t from[20], to[20];
            wprintf(L"Введіть початок періоду (YYYY-MM-DD): ");
            fgetws(from, 20, stdin);  trim_newline(from);
            wprintf(L"Введіть кінець   періоду (YYYY-MM-DD): ");
            fgetws(to, 20, stdin);  trim_newline(to);

            int ic = load_invoices(invoices);
            // Виводимо інвойси:
            list_invoices_period(invoices, ic, from, to);
            break;
        }
        case 12:
            int load_invoices(Invoice invoices[]);
            create_act(); break;
        case 13:
        {  
            wchar_t from[20], to[20];
            wprintf(L"Введіть початок періоду (YYYY-MM-DD): ");
            fgetws(from, 20, stdin);  trim_newline(from);
            wprintf(L"Введіть кінець   періоду (YYYY-MM-DD): ");
            fgetws(to, 20, stdin);  trim_newline(to);

            int ac = load_acts(acts);
            // Виводимо акти:
            list_acts_period(acts, ac, from, to);
            break;
        }
        case 14:
            ensure_payments_file_exists();
            create_payment();
            break;
        case 15:
        {  
            wchar_t from[20], to[20];
            wprintf(L"Введіть початок періоду (YYYY-MM-DD): ");
            fgetws(from, 20, stdin);  trim_newline(from);
            wprintf(L"Введіть кінець   періоду (YYYY-MM-DD): ");
            fgetws(to, 20, stdin);  trim_newline(to);

            // Завантажуємо дані
            int pc = load_payments(payments);
            load_partners(); // для імені партнера у виводі

            // Виводимо платежі:
            list_payments_period(
                payments, pc,
                from, to,
                partners, partner_count
            );
            break;
        }
        case 16:
            load_partners();
            load_services();
            order_count = load_orders(orders);
            list_orders_table(orders, order_count, partners,
                partner_count, services, service_count);
            break;
        case 17: {
            order_count = load_orders(orders);
            invoice_count = load_invoices(invoices);
            act_count = load_acts(acts);
            list_order_summary(
                orders, order_count,
                invoices, invoice_count,
                acts, act_count
            );
            break;
        }
        case 18:
            show_top_margin_orders();
            break;
        case 19:
            add_employee();
            break;
        case 20:
            list_employees();
            break;
        case 21:
            calculate_salary();
            break;
        case 22:
            view_payrolls();
            break;
        case 23:
            edit_employee();
            break;
        case 24: {
            wchar_t period[32];
            clear_input_buffer();
            wprintf(L"Введіть місяць (наприклад, Травень 2025): ");
            fgetws(period, 32, stdin); trim_newline(period);
            report_esv(period);
        } break;

        case 25: {
            wchar_t period[32];
            clear_input_buffer();
            wprintf(L"Введіть місяць (наприклад, Травень 2025): ");
            fgetws(period, 32, stdin); trim_newline(period);
            report_1df(period);
        } break;

        case 26: {
            wchar_t from_[20], to_[20];
            clear_input_buffer();
            wprintf(L"З дати (YYYY-MM-DD): ");
            fgetws(from_, 20, stdin); trim_newline(from_);
            wprintf(L"До дати   (YYYY-MM-DD): ");
            fgetws(to_, 20, stdin); trim_newline(to_);
            report_corporate_tax(from_, to_);
        } break;
        case 27:
            import_orders_csv();
            break;
        case 28: edit_partner();    break;
        case 29: delete_service_prompt(); break;
        case 30: edit_service();    break;
        case 31: {
            // Завантажуємо дані
            order_count = load_orders(orders);
            act_count = load_acts(acts);
            payment_count = load_payments(payments);
            load_partners(); 

            // Генеруємо звіт по дебіторській заборгованості
            report_debtor_aging(
                orders, order_count,
                acts, act_count,
                payments, payment_count,
                partners, partner_count
            );
            break;
        }

        case 32: {
            // Завантажуємо дані
            order_count = load_orders(orders);
            act_count = load_acts(acts);
            payment_count = load_payments(payments);
            load_partners();  // встановить global partner_count

            // Генеруємо звіт по кредиторській заборгованості
            report_creditor_aging(
                orders, order_count,
                acts, act_count,
                payments, payment_count,
                partners, partner_count
            );
            break;
        }

        case 99:
            auto_generate_all_documents();
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
