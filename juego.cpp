#include "miniwin.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

using namespace std;
using namespace miniwin;

class Bala
{
private:
    int x, y;
    int tam = 5;
    int velocidad = 10;

public:
    Bala(int _x, int _y) : x(_x), y(_y) {}

    bool mueve()
    {
        y -= velocidad;
        return y > 0;
    }

    void dibuja() const
    {
        color(BLANCO);
        rectangulo_lleno(x - tam / 2, y - tam / 2, x + tam / 2, y + tam / 2);
    }

    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_tam() const { return tam; }
};

class Enemigo
{
private:
    int x, y;
    int tam = 20;
    bool activo = true;

public:
    Enemigo(int _x, int _y) : x(_x), y(_y) {}

    void dibuja() const
    {
        if (activo)
        {
            color(ROJO);
            rectangulo_lleno(x - tam / 2, y - tam / 2, x + tam / 2, y + tam / 2);
        }
    }

    void mueve()
    {
        y += 2; // Mover hacia abajo
        if (y > 600 + tam)
        {                   // Suponiendo una altura de ventana de 600
            activo = false; // Desactivar si sale de la pantalla
        }
    }

    void colision(const vector<Bala> &balas)
    {
        for (size_t i = 0; i < balas.size(); ++i)
        {
            if (activo && abs(balas[i].get_x() - x) < tam && abs(balas[i].get_y() - y) < tam)
            {
                activo = false;
                break;
            }
        }
    }

    bool esta_activo() const { return activo; }
    int get_y() const { return y; }
};

class Nave
{
private:
    int x, y;
    int tam = 50;
    vector<Bala> balas;

public:
    Nave(int _x, int _y) : x(_x), y(_y) {}

    void dibuja() const
    {
        color(BLANCO);
        rectangulo_lleno(x - tam / 2, y - tam / 2, x + tam / 2, y + tam / 2);
        for (size_t i = 0; i < balas.size(); ++i)
        {
            balas[i].dibuja();
        }
    }

    void mueve(int dx, int dy)
    {
        // x += dx;
        // y += dy;
        // Limitar movimiento a la ventana
        x = max(tam / 2, min(800 - tam / 2, x + dx));
        y = max(tam / 2, min(600 - tam / 2, y + dy));
    }

    void dispara()
    {
        balas.push_back(Bala(x, y - tam / 2));
    }

    void actualizaBalas()
    {
        for (vector<Bala>::iterator it = balas.begin(); it != balas.end();)
        {
            if (!it->mueve())
            {
                it = balas.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    const vector<Bala> &get_balas() const { return balas; }
};

int main()
{
    srand(time(0)); // Inicializar semilla aleatoria
    vredimensiona(800, 600);
    Nave miNave(300, 550);
    vector<Enemigo> enemigos;

    int contador = 0;
    bool fin = false;
    while (!fin)
    {
        if (++contador % 50 == 0)
        {                                                 // Añadir un enemigo cada 100 ciclos
            enemigos.push_back(Enemigo(rand() % 800, 0)); // Posición x aleatoria, y en 0
        }

        borra();
        miNave.dibuja();
        miNave.actualizaBalas();

        for (vector<Enemigo>::iterator it = enemigos.begin(); it != enemigos.end();)
        {
            it->mueve();
            it->colision(miNave.get_balas());
            if (it->esta_activo())
            {
                it->dibuja();
                ++it;
            }
            else
            {
                it = enemigos.erase(it); // Eliminar enemigos inactivos
            }
        }

        refresca();

        int t = tecla();
        switch (t)
        {
        case DERECHA:
            miNave.mueve(20, 0);
            break;
        case IZQUIERDA:
            miNave.mueve(-20, 0);
            break;
        case ARRIBA:
            miNave.dispara();
            break;
        case ESCAPE:
            fin = true;
            break;
        }
        espera(1);
    }

    return 0;
}
