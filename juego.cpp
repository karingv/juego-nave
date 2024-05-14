#include "miniwin.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

#include <sstream>


using namespace std;
using namespace miniwin;

const int WIDTH = 800;
const int HEIGHT = 700;
const int margen = 20;

const int limite_margen = 40;

const int ENEMY_UPDATE = 50;
const int ENEMY_SPEED = 2;
const int ENEMY_SIZE = 20;

const int SHIP_SPEED = 20;
const int SHIP_SIZE = 50;

const int BULLET_SPEED = 10;
const int BULLET_SIZE = 5;

int puntaje = 0; 
int vidas = 150;
int contador_aumentar_vidas = 0;

int enemigos_pasados = 0;


template <typename T>
class StringConverter {
public:
    static std::string to_string(T value) {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
};

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
        if (y > HEIGHT - margen - tam)
        {                   // Suponiendo una altura de ventana de 600
            activo = false; // Desactivar si sale de la pantalla
            enemigos_pasados++;
        }
    }

    void colision(const vector<Bala> &balas)
    {
        for (size_t i = 0; i < balas.size(); ++i)
        {
            if (activo && abs(balas[i].get_x() - x) < tam && abs(balas[i].get_y() - y) < tam)
            {
                activo = false;
                puntaje += 20;
                
                if(puntaje % 100 == 0 && puntaje != 0){
                	vidas += 100;
                	contador_aumentar_vidas ++;
				}
                break;
            }
        }
    }
    

    bool esta_activo() const { return activo; }
    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_enemigos_pasados() const { return enemigos_pasados; }
};

class Nave
{
private:
    int x, y;
    int tam = SHIP_SIZE;
    vector<Bala> balas;
    bool activo = true;

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
        x = max(tam / 2 + (limite_margen+10), min(WIDTH - tam / 2 - (limite_margen+10), x + dx));
        y = max(tam / 2 + limite_margen, min(HEIGHT - tam / 2 - limite_margen, y + dy));
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
    
    void colision(const vector<Enemigo> &enemigos)
   {
    // Verificar colisión con cada enemigo que colisione
    for (int i = 0; i < enemigos.size(); ++i)
    {
        if (abs(enemigos[i].get_x() - x) < tam && abs(enemigos[i].get_y() - y) < tam)
        {
            vidas--; // Reducir la vida en 1
            break;
        }
    }

   }


    const vector<Bala> &get_balas() const { return balas; }
};


int main()
{
    srand(time(0)); // Inicializar semilla aleatoria
    vredimensiona(WIDTH, HEIGHT); //Tamaño de la pantalla	  
	
    Nave miNave(300, HEIGHT - (limite_margen + 30));
    vector<Enemigo> enemigos;

    int contador = 0;
    bool fin = false;
    while (!fin)
    {
        if (++contador % ENEMY_UPDATE == 0)
        {
            enemigos.push_back(Enemigo(limite_margen + rand() % (WIDTH - 2 * (limite_margen + 10)), 70));
        }

        borra();
        dibujar_marco(WIDTH, HEIGHT, margen, 40);     
        
        miNave.dibuja();

        string puntaje_str = "Puntaje: " + StringConverter<int>::to_string(puntaje);
        texto(margen, margen, puntaje_str);
        
        miNave.actualizaBalas();
        miNave.colision(enemigos); 
        
        string vidas_str = "Vidas: " + StringConverter<int>::to_string(vidas + contador_aumentar_vidas);
        texto(margen + 120, margen, vidas_str);
        
        string masVidas_str = "Vidas Aumentadas: " + StringConverter<int>::to_string(contador_aumentar_vidas);
        texto(margen + 240, margen, masVidas_str);
        
        if (vidas == 0) {
        fin = true; 
        break;
    }
    
        if (enemigos_pasados >= 3) {
                fin = true;
                break;
        }
        

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
    
    mensaje("---------------¡GAME OVER!---------------"); 

    return 0;
}
