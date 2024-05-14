#include "miniwin.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

using namespace std;
using namespace miniwin;

const int WIDTH = 800;
const int HEIGHT = 600;

const int ENEMY_UPDATE = 50;
const int ENEMY_SPEED = 2;
const int ENEMY_SIZE = 20;

const int SHIP_SPEED = 20;
const int SHIP_SIZE = 50;

const int BULLET_SPEED = 10;
const int BULLET_SIZE = 5;

class Bala
{
private:
    int x, y;
    int tam = BULLET_SIZE;
    int velocidad = BULLET_SPEED;

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
    int tam = ENEMY_SIZE;
    bool activo = true;

public:
    Enemigo(int _x, int _y) : x(_x), y(_y) {}

    void dibuja() const
    {
        if (activo)
        {
            color(VERDE);
            dibujar_enemigo_alien(x, y, tam, tam, 4, 4);
        }
    }

    void mueve()
    {
        y += ENEMY_SPEED; // Mover hacia abajo
        if (y > HEIGHT + tam)
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
    int tam = SHIP_SIZE;
    vector<Bala> balas;

public:
    Nave(int _x, int _y) : x(_x), y(_y) {}

    void dibuja() const
    {
        color(BLANCO);
        dibujar_nave(x, y, tam, tam);
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
        x = max(tam / 2, min(WIDTH - tam / 2, x + dx));
        y = max(tam / 2, min(HEIGHT - tam / 2, y + dy));
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
    vredimensiona(WIDTH, HEIGHT);
    Nave miNave(300, 550);
    vector<Enemigo> enemigos;

    int contador = 0;
    bool fin = false;
    while (!fin)
    {
        if (++contador % ENEMY_UPDATE == 0)
        {
            enemigos.push_back(Enemigo(rand() % WIDTH, 0));
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
            miNave.mueve(SHIP_SPEED, 0);
            break;
        case IZQUIERDA:
            miNave.mueve(-SHIP_SPEED, 0);
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
