// Evita el interbloqueo tomando los dos tenedores a la vez con
// scoped_lock (no se forma espera circular).

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

const int N = 5;         // filósofos (y tenedores)
const int COMIDAS = 3;   // veces que come cada uno

mutex tenedor[N];   // un mutex por tenedor
mutex salida;       // para que los mensajes no se mezclen
int comio[N];            // cada hilo solo escribe en su posición

void log(int id, const char* msg) {
    lock_guard<mutex> lk(salida);
    cout << "Filosofo " << id << " " << msg << '\n';
}

void filosofo(int id) {
    for (int i = 0; i < COMIDAS; ++i) {
        log(id, "piensa");
        this_thread::sleep_for(chrono::milliseconds(50 + 30 * id));

        log(id, "tiene hambre");
        {
            // Toma ambos tenedores de forma atómica (sin deadlock)
            scoped_lock t(tenedor[id], tenedor[(id + 1) % N]);
            log(id, "come");
            this_thread::sleep_for(chrono::milliseconds(100));
            ++comio[id];
        } // se sueltan los tenedores al salir del bloque
        log(id, "deja los tenedores");
    }
}

int main() {
    thread hilos[N];

    for (int i = 0; i < N; ++i) hilos[i] = thread(filosofo, i);
    for (int i = 0; i < N; ++i) hilos[i].join();

    cout << "\nResumen\n";
    for (int i = 0; i < N; ++i)
        cout << "Filosofo " << i << " comio " << comio[i] << " veces\n";
    cout << "Todos terminaron sin interbloqueo.\n";
    return 0;
}
