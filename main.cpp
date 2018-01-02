#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <iomanip>

using namespace std;

// -------------------- Initialation -------------------- //

#ifndef ADMIN_USERNAME
#define ADMIN_USERNAME "admin"
#endif

#ifndef ADMIN_PASSWORD
#define ADMIN_PASSWORD "admin"
#endif

#ifndef ASK_TO_LOGIN_AGAIN
#define ASK_TO_LOGIN_AGAIN true
#endif

#ifndef IS_UNIX
#define IS_UNIX defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__) || defined(linux) || defined(__linux)
#endif

#ifndef CLEAR_SCREEN_COMMAND
#if IS_UNIX
#define CLEAR_SCREEN_COMMAND "clear"
#else
#define CLEAR_SCREEN_COMMAND "cls"
#endif
#endif

#ifndef PAJAK_PERSEN
#define PAJAK_PERSEN 2.5
#endif

#ifndef LEMBUR_PER_JAM
#define LEMBUR_PER_JAM 100000
#endif

struct Karyawan {
    int id;
    char nama[255];
    char jabatan[255];
    int gaji_pokok;
    Karyawan *next;
};

struct Gaji {
    int id;
    char nama[255];
    int kehadiran;
    int lembur;
    double sub_total;
    double pajak;
    double total;
    int bulan;
    int tahun;
    Gaji *next;
};

Karyawan *first_karyawan;
int increment_karyawan = 0;

Gaji *first_gaji;
int increment_gaji = 0;

Gaji *laporan_gaji;

// -------------------- End of Initialation -------------------- //

// -------------------- General Functions -------------------- //

bool check_login() {
    string username;
    string password;

    cout << "Username: ";
    cin >> username;

    // Hide typed password...
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    cout << "Password: ";
    cin >> password;
    cout << endl;

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return username == ADMIN_USERNAME && password == ADMIN_PASSWORD;

}

void show_main_menu() {
    cout << "Menu" << endl;
    cout << "1) Master Karyawan" << endl;
    cout << "2) Penggajian" << endl;
    cout << "3) Laporan Penggajian" << endl;
    cout << "4) Keluar" << endl << endl;
}

void show_master_karyawan_menu() {
    cout << "Menu" << endl;
    cout << "1) Lihat Semua Karyawan" << endl;
    cout << "2) Tambah Karyawan Baru" << endl;
    cout << "3) Keluar" << endl << endl;
}

void clear_screen() {
    system(CLEAR_SCREEN_COMMAND);
}

void karyawan_file_to_list() {
    FILE *file;
    file = fopen("data/karyawan.bin", "rb");

    if (file != nullptr) {
        first_karyawan = nullptr;

        fseek(file, 0, SEEK_END);

        long file_size = ftell(file);
        rewind(file);

        int num_entries = (int) (file_size / (sizeof(Karyawan)));

        for (int loop = 0; loop < num_entries; ++loop) {
            fseek(file, (sizeof(Karyawan) * loop), SEEK_SET);

            if (first_karyawan == nullptr) {
                first_karyawan = new(Karyawan);

                fread(first_karyawan, sizeof(Karyawan), 1, file);

                first_karyawan->next = nullptr;
            } else {
                Karyawan *index = first_karyawan;

                Karyawan *new_karyawan = new(Karyawan);

                while (index->next != nullptr) {
                    index = index->next;
                }

                fread(new_karyawan, sizeof(Karyawan), 1, file);

                index->next = new_karyawan;
                new_karyawan->next = nullptr;

                Karyawan *last = first_karyawan;

                while (last != nullptr) {
                    increment_karyawan = last->id;
                    last = last->next;
                }
            }
        }
    }

    fclose(file);
}

void gaji_file_to_list() {
    FILE *file;
    file = fopen("data/gaji.bin", "rb");

    if (file != nullptr) {
        first_gaji = nullptr;

        fseek(file, 0, SEEK_END);

        long file_size = ftell(file);
        rewind(file);

        int num_entries = (int) (file_size / (sizeof(Gaji)));

        for (int loop = 0; loop < num_entries; ++loop) {
            fseek(file, (sizeof(Gaji) * loop), SEEK_SET);

            if (first_gaji == nullptr) {
                first_gaji = new(Gaji);

                fread(first_gaji, sizeof(Gaji), 1, file);

                first_gaji->next = nullptr;
            } else {
                Gaji *index = first_gaji;

                Gaji *new_gaji = new(Gaji);

                while (index->next != nullptr) {
                    index = index->next;
                }

                fread(new_gaji, sizeof(Gaji), 1, file);

                index->next = new_gaji;
                new_gaji->next = nullptr;

                Gaji *last = first_gaji;

                while (last != nullptr) {
                    increment_gaji = last->id;
                    last = last->next;
                }
            }
        }
    }

    fclose(file);
}

// ------------------ End of General Functions ------------------ //

// ------------------ Activities ------------------ //

bool login_activity(bool ask_to_login_again = false) {
    if (ask_to_login_again) {
        char is_want_to_login_again;
        cout << "Apakah Anda ingin login lagi? (Y / N): ";
        cin >> is_want_to_login_again;

        if (is_want_to_login_again == 'N' || is_want_to_login_again == 'n') {
            return false;
        } else {
            clear_screen();
        }
    }

    bool is_login_success = false;

    while (true) {
        bool can_login = check_login();

        if (can_login) {
            is_login_success = true;
            break;
        } else {
            char try_login_again_answer;
            cout << "Username atau password salah. Coba lagi? (Y / N): ";
            cin >> try_login_again_answer;

            if (try_login_again_answer == 'N' || try_login_again_answer == 'n') {
                break;
            } else {
                clear_screen();
            }
        }
    }

    return is_login_success;
}

void create_new_karyawan() {
    increment_karyawan++;

    char nama[255];
    char jabatan[255];
    int gaji_pokok;

    cout << "Tambah Karyawan Baru" << endl;
    cout << "Nama: ";
    cin >> nama;
    cout << "Jabatan: ";
    cin >> jabatan;
    cout << "Gaji Pokok: ";
    cin >> gaji_pokok;

    if (first_karyawan == nullptr) {
        first_karyawan = new(Karyawan);
        first_karyawan->id = increment_karyawan;
        strcpy(first_karyawan->nama, nama);
        strcpy(first_karyawan->jabatan, jabatan);
        first_karyawan->gaji_pokok = gaji_pokok;
        first_karyawan->next = nullptr;
    } else {
        Karyawan *index = first_karyawan;

        while (index->next != nullptr) {
            index = index->next;
        }

        Karyawan *new_karyawan = new(Karyawan);
        new_karyawan->id = increment_karyawan;
        strcpy(new_karyawan->nama, nama);
        strcpy(new_karyawan->jabatan, jabatan);
        new_karyawan->gaji_pokok = gaji_pokok;
        new_karyawan->next = nullptr;

        index->next = new_karyawan;
    }

    FILE *file;
    file = fopen("data/karyawan.bin", "wb");

    if (file != nullptr) {
        Karyawan *index = first_karyawan;

        while (index != nullptr) {
            fseek(file, 0, SEEK_END);

            fwrite(index, sizeof(Karyawan), 1, file);

            index = index->next;
        }
    }

    fclose(file);

    karyawan_file_to_list();
}

void view_karyawan() {
    cout << "Daftar Karyawan" << endl;

    Karyawan *index = first_karyawan;

    while (index != nullptr) {
        cout << "ID: " << index->id << endl;
        cout << "Nama: " << index->nama << endl;
        cout << "Jabatan: " << index->jabatan << endl;
        cout << "Gaji Pokok: " << index->gaji_pokok << endl;

        index = index->next;
    }

    system("read");
}

void master_karyawan_activity() {
    bool quit = false;

    while (! quit) {
        clear_screen();

        int selected_menu;

        show_master_karyawan_menu();

        cout << "Pilih: ";

        cin >> selected_menu;

        switch (selected_menu) {
            case 1:
                clear_screen();
                view_karyawan();
                break;
            case 2:
                clear_screen();
                create_new_karyawan();
                break;
            default:
                quit = true;
                break;
        }
    }
}

void penggajian_activity() {
    clear_screen();

    bool find_karyawan = false;
    Karyawan *karyawan;
    int karyawan_id;

    char nama[255];
    int kehadiran;
    int lembur;
    double sub_total;
    double pajak;
    double total;
    int bulan;
    int tahun;

    cout << "Penggajian" << endl;

    cout << "ID Karyawan: ";
    cin >> karyawan_id;

    Karyawan *index = first_karyawan;

    while (index != nullptr) {
        if (karyawan_id == index->id) {
            find_karyawan = true;
            karyawan = index;
            break;
        }

        index = index->next;
    }

    if (! find_karyawan) {
        clear_screen();

        cout << "Tidak bisa menemukan karyawan dengan ID: " << karyawan_id << endl;

        system("read");

        return;
    }

    cout << "Bulan: ";
    cin >> bulan;

    cout << "Tahun: ";
    cin >> tahun;

    cout << "Jumlah kehadiran: ";
    cin >> kehadiran;

    cout << "Jumlah jam lembur: ";
    cin >> lembur;

    sub_total = karyawan->gaji_pokok + (lembur * LEMBUR_PER_JAM);
    pajak = (sub_total * PAJAK_PERSEN) / 100;
    total = sub_total - pajak;

    increment_gaji++;

    if (first_gaji == nullptr) {
        first_gaji = new(Gaji);
        first_gaji->id = increment_gaji;
        strcpy(first_gaji->nama, karyawan->nama);
        first_gaji->kehadiran = kehadiran;
        first_gaji->lembur = lembur;
        first_gaji->sub_total = sub_total;
        first_gaji->pajak = pajak;
        first_gaji->total = total;
        first_gaji->bulan = bulan;
        first_gaji->tahun = tahun;
        first_gaji->next = nullptr;
    } else {
        Gaji *index = first_gaji;

        while (index->next != nullptr) {
            index = index->next;
        }

        Gaji *new_gaji = new(Gaji);
        new_gaji->id = increment_gaji;
        strcpy(first_gaji->nama, karyawan->nama);
        first_gaji->kehadiran = kehadiran;
        first_gaji->lembur = lembur;
        first_gaji->sub_total = sub_total;
        first_gaji->pajak = pajak;
        first_gaji->total = total;
        first_gaji->bulan = bulan;
        first_gaji->tahun = tahun;
        first_gaji->next = nullptr;

        index->next = new_gaji;
    }

    FILE *file;
    file = fopen("data/gaji.bin", "wb");

    if (file != nullptr) {
        Gaji *index = first_gaji;

        while (index != nullptr) {
            fseek(file, 0, SEEK_END);

            fwrite(index, sizeof(Gaji), 1, file);

            index = index->next;
        }
    }

    fclose(file);

    gaji_file_to_list();
}

void laporan_penggajian_activity() {
    clear_screen();
    laporan_gaji = nullptr;

    int bulan;
    int tahun;

    cout << "Bulan: ";
    cin >> bulan;

    cout << "Tahun: ";
    cin >> tahun;

    Gaji *index = first_gaji;
    Gaji *tail = nullptr;

    while (index != nullptr) {
        if (index->bulan == bulan && index->tahun == tahun) {
            if (laporan_gaji == nullptr) {
                laporan_gaji = index;
                laporan_gaji->next = tail;
            } else {
                Gaji *new_gaji = index;

                new_gaji->next = nullptr;
                tail->next = new_gaji;
                tail = new_gaji;
            }
        }

        index = index->next;
    }

    index = laporan_gaji;

    clear_screen();

    cout << "Data Gaji Bulan: " << bulan << " Tahun: " << tahun << endl;

    while (index != nullptr) {
        cout << "Nama: " << index->nama << endl;
        cout << "Jumlah kehadiran: " << index->kehadiran << endl;
        cout << "Jumlah jam lembur: " << index->lembur << endl;

        cout << setprecision(2);
        cout << fixed;

        cout << "Subtotal: " << index->sub_total << endl;
        cout << "Pajak: " << index->pajak << endl;
        cout << "Total: " << index->total << endl << endl;

        index = index->next;
    }

    system("read");
}

// ------------------ End of Activities ------------------ //

// ------------------ Main App ------------------ //

int main() {
    karyawan_file_to_list();
    gaji_file_to_list();

    cout << "Selamat datang, silakan login." << endl;

    // LOGIN ACTIVITY...
    bool is_login_success = login_activity();

    if (! is_login_success) {
        cout << "Maaf, Anda tidak bisa menggunakan aplikasi ini sebelum login.";

        // Exit app...
        return 0;
    }
    // END OF LOGIN ACTIVITY...

    // MAIN ACTIVITY...
    bool quit = false;

    while (! quit) {
        clear_screen();

        int selected_menu;

        show_main_menu();

        cout << "Pilih: ";

        cin >> selected_menu;

        switch (selected_menu) {
            case 1:
                master_karyawan_activity();
                break;
            case 2:
                penggajian_activity();
                break;
            case 3:
                laporan_penggajian_activity();
                break;
            default:
                clear_screen();

                cout << "Anda telah keluar dari aplikasi." << endl;

                quit = ! login_activity(ASK_TO_LOGIN_AGAIN);

                break;
        }
    }

    return 0;
}

// ------------------ End of Main App ------------------ //
