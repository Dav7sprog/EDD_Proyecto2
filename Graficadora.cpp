#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <cstdlib>

using namespace std;

string ReadLine(const string& prompt) {
    string resultado;
    cout << prompt;
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, resultado);
    size_t ini = resultado.find_first_not_of(" \t");
    size_t fin = resultado.find_last_not_of(" \t");
    if (ini == string::npos) return "";
    return resultado.substr(ini, fin - ini + 1);
}

//Matriz dispersa

struct NodoCol{
    public:
    int Col;
    string Color;
    NodoCol* SigC;
    NodoCol(int c, string color){
        Col = c;
        Color = color;
        SigC = nullptr;
    }
};

struct NodoFila{
    public:
    int Fila;
    NodoCol* Col1;
    NodoFila* SigFila;
    NodoFila(int f){
        Fila = f;
        Col1 = nullptr;
        SigFila = nullptr;
    }
};

struct Matriz_Dispersa{
    public:
    NodoFila* Fila1;
    Matriz_Dispersa(){
        Fila1 = nullptr;
    }

    void Insertar(int fila, int col, string color){
        NodoFila* f = Fila1; 
        NodoFila* fila_ant = nullptr;
        while(f != nullptr && f->Fila < fila){
            fila_ant = f;
            f = f -> SigFila;
        }
        if(f == nullptr || f->Fila != fila){
            NodoFila* nodofila = new NodoFila(fila);
            nodofila->SigFila = f;
            if(fila_ant == nullptr){
                Fila1 = nodofila;
            }else{
                fila_ant->SigFila = nodofila;
            }
            f = nodofila;
        }
        NodoCol* c = f->Col1;
        NodoCol* col_ant = nullptr;
        while(c != nullptr && c->Col < col){
            col_ant = c;
            c = c->SigC;
        }
        if(c != nullptr && c->Col == col){
            c->Color = color;
        }else{
            NodoCol* nodo_col = new NodoCol(col, color);
            nodo_col->SigC = c;
            if(col_ant == nullptr){
                f->Col1 = nodo_col;
            }else{
                col_ant->SigC = nodo_col;
            }
        }
    }
    string getColor(int fila, int col){
        NodoFila* f = Fila1;
        while(f != nullptr){
            if(f->Fila == fila){
                NodoCol* c = f->Col1;
                while(c != nullptr){
                    if(c->Col == col){
                        return c->Color;
                    }
                    c = c->SigC;
                }
                return "";
            }
            f = f->SigFila;
        }
        return "";
    }
    
    void getDims(int& maxF, int& maxC){
        maxF = 0;
        maxC = 0;
        NodoFila* f = Fila1;
        while(f != nullptr){
            if(f->Fila > maxF){
                maxF = f->Fila;
            }
            NodoCol* c = f->Col1;
            while (c != nullptr){
                if(c->Col > maxC){
                    maxC = c->Col;
                }
                c = c->SigC;
            }
            f = f->SigFila;
        }
    }
    void Mostrar(){
        NodoFila* f = Fila1;
        while(f != nullptr){
            NodoCol* c = f->Col1;
            while(c != nullptr){
                cout<< " Fila:" << f->Fila << " Columna:" << c->Col << " Color:" << c->Color <<"\n";
                c = c->SigC;
            }
            f = f->SigFila;
        }
    }
    string ToDot(int id_Capa){
        string dot = "subgraph cluster_matriz_" + to_string(id_Capa) + "{\n";
        dot += "label=\"Matriz Capa " + to_string(id_Capa) + "\"; \n";
        NodoFila* f = Fila1;
        while (f != nullptr){
            string filaId = "f" + to_string(id_Capa) + "_" + to_string(f->Fila);
            dot +="  " + filaId + " [label=\"Fila " + to_string(f->Fila) + "\", shape=box];\n";
            NodoCol* c = f->Col1;
            string prevId = filaId;
            while (c != nullptr) {
                string colId = "c" + to_string(id_Capa) + "_" + to_string(f->Fila) + "_" + to_string(c->Col);
                dot += "  " + colId + " [label=\"Col:" + to_string(c->Col) + "\\n" + c->Color + "\", shape=ellipse];\n";
                dot += "  " + prevId + " -> " + colId + ";\n";
                prevId = colId;
                c = c->SigC;
            }
            f = f->SigFila;
        }
        dot += "}\n";
        return dot;
    }
};

//Capa

struct CapaG{
    int ID;
    Matriz_Dispersa* matriz;
    CapaG(int id){
        ID = id;
        matriz = new Matriz_Dispersa();
    }
};

//ABB PARA CAPAS

struct NodoABB{
    CapaG* Capa;
    NodoABB* left;
    NodoABB* right;
    NodoABB(CapaG* c){
        Capa = c;
        left = nullptr;
        right = nullptr;
    }
};

struct CapasABB{
    NodoABB* root; 
    CapasABB(){
        root = nullptr;
    }

    NodoABB* InsertarN(NodoABB* nodo, CapaG* capa){
        if(nodo == nullptr){
            return new NodoABB(capa);
        }
        if(capa->ID < nodo->Capa->ID){
            nodo->left = InsertarN(nodo->left, capa);
        }
        else if(capa->ID > nodo->Capa->ID){
            nodo->right = InsertarN(nodo->right, capa);
        }else{
            cout << "Capa con ID "<< capa->ID << " ya existente \n";
        }
        return nodo;
    }

    void Insert(CapaG* capa){
        root = InsertarN(root, capa);
    }    

    CapaG* BuscarC(NodoABB* nodo, int id){
        if(nodo == nullptr){
            return nullptr;
        }
        if(id == nodo->Capa->ID){
            return nodo->Capa;
        } 
        if(id < nodo->Capa->ID){
            return BuscarC(nodo->left, id);
        }
        return BuscarC(nodo->right, id);
    }

    CapaG* Buscar(int id){
        return BuscarC(root, id);
    }

    void Preorden(NodoABB* nodo, vector<CapaG*>& resultado){
        if(nodo == nullptr){
            return;
        }
        resultado.push_back(nodo->Capa);
        Preorden(nodo->left, resultado);
        Preorden(nodo->right, resultado);
    }

    void Inoden(NodoABB* nodo, vector<CapaG*>& resultado){
        if(nodo == nullptr){
            return;
        }
        Inoden(nodo->left, resultado);
        resultado.push_back(nodo->Capa);
        Inoden(nodo->right, resultado);
    }

    void Postorden(NodoABB* nodo, vector<CapaG*>& resultado){
        if(nodo == nullptr){
            return;
        }
        Postorden(nodo->left, resultado);
        Postorden(nodo->right, resultado);
        resultado.push_back(nodo->Capa);
    }

    string ToDotRec(NodoABB* nodo){
        if (nodo == nullptr){
            return "";
        }
        string dot = "  capa_" + to_string(nodo->Capa->ID) +
                     " [label=\"Capa " + to_string(nodo->Capa->ID) + "\"];\n";
        if (nodo->left) {
            dot += "  capa_" + to_string(nodo->Capa->ID) + " -> capa_" + to_string(nodo->left->Capa->ID) + ";\n";
            dot += ToDotRec(nodo->left);
        }
        if (nodo->right) {
            dot += "  capa_" + to_string(nodo->Capa->ID) + " -> capa_" + to_string(nodo->right->Capa->ID) + ";\n";
            dot += ToDotRec(nodo->right);
        }
        return dot;
    }

    string ToDot(){
        string dot = "digraph ABBCapas {\n  node [shape=box];\n";
        dot += ToDotRec(root);
        dot += "}\n";
        return dot;
    }
};

//imagen

struct Nodo_Capa{
    public:
    CapaG* capa;
    Nodo_Capa* Sig;
    Nodo_Capa(CapaG* c){
        capa = c;
        Sig = nullptr;
    }
};

struct Imagen{
    public:
    int ID;
    Nodo_Capa* capas;
    Imagen(int id){
        ID = id;
        capas = nullptr;
    }

    void Insertar_Capa(CapaG* c){
        Nodo_Capa* nuevo = new Nodo_Capa(c);
        if(capas == nullptr){
            capas = nuevo;
            return;
        }
        Nodo_Capa* aux = capas;
        while(aux->Sig != nullptr){
            aux = aux->Sig;
        }
        aux->Sig = nuevo;
    }

    bool Eliminar_Capa(int id){
        Nodo_Capa* aux = capas;
        Nodo_Capa* ant = nullptr;
        while(aux != nullptr){
            if(aux->capa->ID == id){
                if(ant == nullptr){
                    capas = aux->Sig;
                }else{
                    ant->Sig = aux->Sig;
                }
                delete aux;
                return true;
            }
            ant = aux;
            aux = aux->Sig;
        }
        return false;
    }
};

//Lista circular 

struct Nodo_Img{
    public:
    Imagen* Img;
    Nodo_Img* Sig;
    Nodo_Img* Ant;
    Nodo_Img(Imagen* img){
        Img = img;
        Sig = nullptr;
        Ant = nullptr;
    }
};

struct Listas_img{
    public:
    Nodo_Img* head;
    int Size;
    Listas_img(){
        head = nullptr;
        Size = 0;
    }

    void Insertar(Imagen* img){
        Nodo_Img* nuevo = new Nodo_Img(img);
        if(head == nullptr){
            head = nuevo;
            nuevo->Sig = nuevo;
            nuevo->Ant = nuevo;
        }else{
            Nodo_Img* ultimo = head->Ant;
            ultimo->Sig = nuevo;
            nuevo->Ant = ultimo;
            nuevo->Sig = head;
            head->Ant = nuevo;
        }
        Size++;
    }

    Imagen* Buscar(int id){
        if(head == nullptr){
            return nullptr;
        }
        Nodo_Img* aux = head;
        do{
            if(aux->Img->ID == id){
                return aux->Img;
            }
            aux = aux->Sig;
        }while(aux != head);
        return nullptr;
    }

    bool Elimiar(int id){
        if(head == nullptr){
            return false;
        }
        Nodo_Img* aux = head;
        do{
            if(aux->Img->ID == id){
                if(Size == 1){
                    head = nullptr;
                }else{
                    aux->Ant->Sig = aux->Sig;
                    aux->Sig->Ant = aux->Ant;
                    if(aux == head){
                        head = aux->Sig;
                    }
                }
                delete aux;
                Size--;
                return true;
            }
            aux = aux->Sig;
        }while(aux != head);
        return false;
    }

    string ToDot(){
        if (head == nullptr){
            return "digraph ListaImagenes { }\n";
        }
        string dot = "digraph ListaImagenes {\n  rankdir=LR;\n  node [shape=box];\n";
        Nodo_Img* aux = head;
        do {
            dot += "  img_" + to_string(aux->Img->ID) + " [label=\"Imagen " + to_string(aux->Img->ID) + "\"];\n";
            Nodo_Capa* nc = aux->Img->capas;
            string prev = "img_" + to_string(aux->Img->ID);
            while (nc != nullptr) {
                string cId = "ic_" + to_string(aux->Img->ID) + "_" + to_string(nc->capa->ID);
                dot += "  " + cId + " [label=\"Capa " + to_string(nc->capa->ID) + "\", shape=ellipse];\n";
                dot += "  " + prev + " -> " + cId + " [style=dashed];\n";
                prev = cId; nc = nc->Sig;
            }
            aux = aux->Sig;
        } while (aux != head);
        aux = head;
        do {
            dot += "  img_" + to_string(aux->Img->ID) + " -> img_" + to_string(aux->Sig->Img->ID) + " [label=\"sig\"];\n";
            dot += "  img_" + to_string(aux->Img->ID) + " -> img_" + to_string(aux->Ant->Img->ID) + " [label=\"ant\",style=dotted];\n";
            aux = aux->Sig;
        } while (aux != head);
        dot += "}\n";
        return dot;
    }
};

//Usuario

struct Nodo_ListaImg{
    public:
    Imagen* Img;
    Nodo_ListaImg* Sig;
    Nodo_ListaImg(Imagen* img){
        Img = img;
        Sig = nullptr;
    }
};

struct Usuario{
    public:
    string Nombre;
    Nodo_ListaImg* Listaimg;
    Usuario(string n){
        Nombre = n;
        Listaimg = nullptr;
    }

    void Agregar_img(Imagen* img){
        Nodo_ListaImg* nuevo = new Nodo_ListaImg(img);
        if(Listaimg == nullptr){
            Listaimg = nuevo;
            return;
        }
        Nodo_ListaImg* aux = Listaimg;
        while(aux->Sig != nullptr){
            aux = aux->Sig;
        }
        aux->Sig = nuevo;
    }

    bool Eliminar_img(int id){
        Nodo_ListaImg* aux = Listaimg;
        Nodo_ListaImg* ant = nullptr;
        while(aux != nullptr){
            if(aux->Img->ID == id){
                if(ant == nullptr){
                    Listaimg = aux->Sig;
                }else{
                    ant->Sig = aux->Sig;
                }
                delete aux;
                return true;
            }
            ant = aux;
            aux = aux->Sig;
        }
        return false;
    }
};

//ABB Usuarios

struct NodoUsuarios{
    public:
    Usuario* User;
    NodoUsuarios* left;
    NodoUsuarios* right;

    NodoUsuarios(Usuario* user){
        User = user; 
        left = nullptr;
        right = nullptr;
    }
};

struct ABBUsuarios{
    public:
    NodoUsuarios* root;
    ABBUsuarios(){
        root = nullptr;
    }

    NodoUsuarios* InsertarU(NodoUsuarios* nodo, Usuario* user){
        if(nodo == nullptr){
            return new NodoUsuarios(user);
        }
        if(user->Nombre < nodo->User->Nombre){
            nodo->left = InsertarU(nodo->left, user);
        }
        else if(user->Nombre > nodo->User->Nombre){
            nodo->right = InsertarU(nodo->right, user);
        }else{
            cout << "Usuario " << user->Nombre << " ya existe\n";
        }
        return nodo;
    }

    void Insertar(Usuario* user){
        root = InsertarU(root, user);
    }

    Usuario* BuscarU(NodoUsuarios* nodo, string nombre){
        if (nodo == nullptr) {
            return nullptr;
        }
        if (nombre == nodo->User->Nombre){
            return nodo->User;
        }
        if (nombre < nodo->User->Nombre){
            return BuscarU(nodo->left, nombre);
        }
        return BuscarU(nodo->right, nombre);
    }

    Usuario* Buscar(string nombre){
        return BuscarU(root, nombre);
    }

    NodoUsuarios* min(NodoUsuarios* nodo){
        while(nodo->left != nullptr){
            nodo = nodo->left;
        }
        return nodo;
    }

    NodoUsuarios* EliminarU(NodoUsuarios* nodo, string nombre){
        if (nodo == nullptr){
            return nullptr;
        }
        if (nombre < nodo->User->Nombre){
            nodo->left = EliminarU(nodo->left, nombre);
        }
        else if (nombre > nodo->User->Nombre){ 
            nodo->right = EliminarU(nodo->right, nombre);
        }
        else {
            if (nodo->left == nullptr) {NodoUsuarios* tmp = nodo->right; delete nodo; return tmp; }
            if (nodo->right == nullptr) {NodoUsuarios* tmp = nodo->left; delete nodo; return tmp; }
            NodoUsuarios* suc = min(nodo->right);
            nodo->User = suc->User;
            nodo->right = EliminarU(nodo->right, suc->User->Nombre);
        }
        return nodo;
    }

    void Eliminar(string nombre){
        root = EliminarU(root, nombre);
    }

    string ToDotU(NodoUsuarios* nodo){
        if (nodo == nullptr){
            return "";
        }
        string dot = "  user_" + nodo->User->Nombre + " [label=\"" + nodo->User->Nombre + "\"];\n";
        Nodo_ListaImg* list = nodo->User->Listaimg;
        string prev = "user_" + nodo->User->Nombre;
        while (list != nullptr) {
            string lid = "ui_" + nodo->User->Nombre + "_" + to_string(list->Img->ID);
            dot += "  " + lid + " [label=\"img " + to_string(list->Img->ID) + "\", shape=ellipse, color=red];\n";
            dot += "  " + prev + " -> " + lid + " [style=dashed, color=red];\n";
            prev = lid; list = list->Sig;
        }
        if (nodo->left) {
            dot += "  user_" + nodo->User->Nombre + " -> user_" + nodo->left->User->Nombre + ";\n";
            dot += ToDotU(nodo->left);
        }
        if (nodo->right) {
            dot += "  user_" + nodo->User->Nombre + " -> user_" + nodo->right->User->Nombre + ";\n";
            dot += ToDotU(nodo->right);
        }
        return dot;   
    }

    string ToDot(){
        string dot = "digraph ABBUsuarios {\n  node [shape=box];\n";
        dot += ToDotU(root);
        dot += "}\n";
        return dot;       
    }
};

//Generacion de imagenes

void GenerarImg(const vector<CapaG*>& capas, const string archivodesalida){
    if (capas.empty()) {
        vector<unsigned char> px = {0, 0, 0};
        stbi_write_png(archivodesalida.c_str(), 1, 1, 3, px.data(), 3);
        cout << "Imagen generada (sin capas): " << archivodesalida << endl;
        return;
    }

    int maxFila = 0, maxCol = 0;
    for (CapaG* c : capas) {
        int mf, mc;
        c->matriz->getDims(mf, mc);
        if (mf > maxFila) maxFila = mf;
        if (mc > maxCol)  maxCol  = mc;
    }

    int filas = maxFila + 1;
    int cols  = maxCol  + 1;

    vector<vector<string>> canvas(filas, vector<string>(cols, "#FFFFFF"));

    for (CapaG* c : capas) {
        NodoFila* f = c->matriz->Fila1;
        while (f != nullptr) {
            NodoCol* col = f->Col1;
            while (col != nullptr) {
                if (f->Fila < filas && col->Col < cols)
                    canvas[f->Fila][col->Col] = col->Color;
                col = col->SigC;
            }
            f = f->SigFila;
        }
    }
    
    vector<unsigned char> pixels(filas * cols * 3);
    for (int i = 0; i < filas; i++) {
    for (int j = 0; j < cols; j++) {
        string hex = canvas[i][j];
        if (hex[0] == '#') hex = hex.substr(1);
        while (hex.size() < 6) hex = "0" + hex;
        int idx = (i * cols + j) * 3;
        pixels[idx]   = stoi(hex.substr(0,2), nullptr, 16);
        pixels[idx+1] = stoi(hex.substr(2,2), nullptr, 16);
        pixels[idx+2] = stoi(hex.substr(4,2), nullptr, 16);
    }
}
stbi_write_png(archivodesalida.c_str(), cols, filas, 3, pixels.data(), cols * 3);
cout << "Imagen generada: " << archivodesalida << " (" << cols << "x" << filas << " px)\n";
}

void Generacion_R(CapasABB& abb, int numC, const string& tipo, const string& salida){
    vector<CapaG*> todo;
    if(tipo == "Inorden"){
        abb.Inoden(abb.root, todo);
    }
    else if (tipo == "Preorden"){
        abb.Preorden(abb.root, todo);
    }
    else if (tipo == "Postorden"){
        abb.Postorden(abb.root, todo);
    }else { 
        cout << "Tipo invalido. Usa: Inorden, Preorden, Postorden\n"; return; 
    }
    if (numC > (int)todo.size()){
        numC = todo.size();
    }
    vector<CapaG*> seleccionadas(todo.begin(), todo.begin() + numC);
    GenerarImg(seleccionadas, salida);
}

void Generar_I(Listas_img& lista, int id, const string& salida){
    Imagen* img = lista.Buscar(id);
    if (!img){
        cout << "Imagen " << id << " no encontrada.\n";
        return; 
    }
    vector<CapaG*> capas;
    Nodo_Capa* nc = img->capas;
    while (nc != nullptr) { 
        capas.push_back(nc->capa); nc = nc->Sig;
    }
    GenerarImg(capas, salida);  
}

void Generar_C(CapasABB& abb, int id, const string& salida){
    CapaG* c = abb.Buscar(id);
    if (!c) { 
        cout << "Capa " << id << " no encontrada.\n"; return; 
    }
    vector<CapaG*> capas = {c};
    GenerarImg(capas, salida);
}

void Generar_U(ABBUsuarios& abb, Listas_img& lista, const string& nombre, int id, const string& salida){
    Usuario* u = abb.Buscar(nombre);
    if (!u) { 
        cout << "Usuario " << nombre << " no encontrado.\n"; return; 
    }
    Nodo_ListaImg* list = u->Listaimg;
    bool tiene = false;
    while (list != nullptr) {
        if (list->Img->ID == id) { 
            tiene = true; break; 
        }
        list = list->Sig;
    }
    if (!tiene) { 
        cout << "El usuario " << nombre << " no tiene la imagen " << id << ".\n"; return; 
    }
    Generar_I(lista, id, salida);    
}

//CRUD

void CRUDAgregar_U(ABBUsuarios& abb){
    string nombre;
    cout << "Nombre del nuevo usuario: ";
    cin >> nombre;
    if(abb.Buscar(nombre) != nullptr){
        cout<< "El usuario ya existe\n";
        return;
    }
    abb.Insertar(new Usuario(nombre));
    cout << "Usuario " << nombre << " agregado\n";
}

void CRUDEliminar_U(ABBUsuarios& abb, Listas_img& lista){
    string nombre;
    cout << "Nombre del usuario a eliminar: ";
    cin >> nombre;
    Usuario* u = abb.Buscar(nombre);
    if(!u){
        cout << "Usuario no encontrado\n";
        return;
    }
    Nodo_ListaImg* list = u->Listaimg;
    while(list != nullptr){
        lista.Elimiar(list->Img->ID);
        list = list->Sig;
    }
    abb.Eliminar(nombre);
    cout << "Usuario: " << nombre << " eliminado\n";
}

void CRUDModificar_U(ABBUsuarios& abb){
    string nombre;
    cout << "Nombre del usuario a modificar: "; cin >> nombre;
    Usuario* u = abb.Buscar(nombre);
    if (!u) { 
        cout << "Usuario no encontrado.\n"; return; 
    }
    string nuevoNombre;
    cout << "Nuevo nombre: "; cin >> nuevoNombre;
    if (abb.Buscar(nuevoNombre) != nullptr) {
        cout << "Ese nombre ya existe\n"; return; 
    }
    Usuario* nuevo = new Usuario(nuevoNombre);
    nuevo->Listaimg = u->Listaimg;
    abb.Eliminar(nombre);
    abb.Insertar(nuevo);
    cout << "Usuario modificado: " << nuevoNombre << ".\n";
}

//Imagenes

void CRUDAgregar_I(Listas_img& lista, CapasABB& abb, ABBUsuarios& abb_u){
    int id;
    string nombreUser;
    cout << "ID de la nueva imagen: "; 
    cin >> id;
    if (lista.Buscar(id) != nullptr) {
        cout << "La imagen con ese ID ya existe\n";
        return; 
    }
    cout << "Nombre del usuario al que se asignara: "; 
    cin >> nombreUser;
    Usuario* u = abb_u.Buscar(nombreUser);
    if (!u) { 
        cout << "Usuario no encontrado\n"; 
        return; 
    }
    Imagen* img = new Imagen(id);
    cout << "Ingresa IDs de capas separados por espacio, termina con -1:\n";
    int idCapa;
    while (cin >> idCapa && idCapa != -1) {
        CapaG* c = abb.Buscar(idCapa);
        if (c){
            img->Insertar_Capa(c);
        }
        else cout << "Capa " << idCapa << " no existe\n";
    }
    lista.Insertar(img);
    u->Agregar_img(img);
    cout << "Imagen " << id << " agregada y asignada a " << nombreUser << ".\n";
}

void CRUDEliminar_I(Listas_img& lista, ABBUsuarios& abb_u){
    string nombreUser;
    int id;
    cout << "Nombre del usuario: ";
    cin >> nombreUser;
    cout <<"ID de la imagen a eliminar: ";
    cin >> id;
    Usuario* u = abb_u.Buscar(nombreUser);
    if(!u){
        cout << "Usuario no encontrado \n";
    }
    u->Eliminar_img(id);
    lista.Elimiar(id);
    cout << "Imagen " <<id <<" eliminada\n";
}

//Carga masiva

void Cargar_C(const string& archivo, CapasABB& abb){
    ifstream f(archivo);
    if (!f.is_open()) {
        cout << "No se pudo abrir " << archivo << endl;
        return; 
    }
    string linea;
    CapaG* capaActual = nullptr;
    while (getline(f, linea)) {
        if (linea.empty()) {
            continue;
        }
        if (linea.find('{') != string::npos) {
            int id = stoi(linea.substr(0, linea.find('{')));
            capaActual = new CapaG(id);
            continue;
        }
        if (linea.find('}') != string::npos) {
            if (capaActual) { 
                abb.Insert(capaActual); 
                capaActual = nullptr; 
            }
            continue;
        }
        if (capaActual && linea.find(',') != string::npos) {
            stringstream ss(linea);
            string tok;
            getline(ss, tok, ','); int fila = stoi(tok);
            getline(ss, tok, ','); int col  = stoi(tok);
            getline(ss, tok, ';'); string color = tok;
            capaActual->matriz->Insertar(fila, col, color);
        }
    }
    f.close();
    cout << "Capas cargadas desde " << archivo << endl;
}

void Cargar_I(const string& archivo, Listas_img& lista, CapasABB& c_abb){
    ifstream f(archivo);
    if (!f.is_open()) { 
        cout << "No se pudo abrir " << archivo << endl; 
        return; 
    }
    string linea;
    while (getline(f, linea)) {
        if (linea.empty()){
            continue;
        }
        size_t posL = linea.find('{');
        size_t posR = linea.find('}');
        int idImg = stoi(linea.substr(0, posL));
        if (lista.Buscar(idImg) != nullptr) {
            cout << "Imagen " << idImg << " ya existe\n"; 
            continue; 
        }
        Imagen* img = new Imagen(idImg);
        string contenido = linea.substr(posL + 1, posR - posL - 1);
        if (!contenido.empty()) {
            stringstream ss(contenido);
            string tok;
            while (getline(ss, tok, ',')) {
                int idCapa = stoi(tok);
                CapaG* c = c_abb.Buscar(idCapa);
                if (c){
                    img->Insertar_Capa(c);
                }else{
                    cout << "Capa " << idCapa << " no encontrada para imagen " << idImg << endl;
                }
            }
        }
        lista.Insertar(img);
    }
    f.close();
    cout << "Imagenes cargadas desde " << archivo << endl;
}

void Cargar_U(const string& archivo, ABBUsuarios& abb_u, Listas_img& lista){
    ifstream f(archivo);
    if (!f.is_open()) { 
        cout << "No se pudo abrir " << archivo << endl; 
        return; 
    }
    string linea;
    while (getline(f, linea)) {
        if (linea.empty()) {
            continue;
        }
        size_t pos = linea.find(':');
        string nombre = linea.substr(0, pos);
        string resto  = linea.substr(pos + 1);
        if (!resto.empty() && resto.back() == ';'){
            resto.pop_back();
        }
        Usuario* u = new Usuario(nombre);
        if (!resto.empty()) {
            stringstream ss(resto);
            string tok;
            while (getline(ss, tok, ',')) {
                int idImg = stoi(tok);
                Imagen* img = lista.Buscar(idImg);
                if (img) {
                    u->Agregar_img(img);
                }else{
                    cout << "Imagen " << idImg << " no encontrada para usuario " << nombre << endl;
                }
            }
        }
        abb_u.Insertar(u);
    }
    f.close();
    cout << "Usuarios cargados desde " << archivo << endl;
}

//Graphviz para nodos

string dotCapaIndividual(CapasABB& abb, int idCapa) {
    CapaG* c = abb.Buscar(idCapa);
    if (!c) return "// Capa " + to_string(idCapa) + " no encontrada\n";

    string dot = "digraph Capa_" + to_string(idCapa) + " {\n";
    dot += "  rankdir=LR;\n";
    dot += "  node [fontname=\"Helvetica\"];\n\n";

    string matId = "mat_" + to_string(idCapa);
    dot += "  " + matId + " [label=\"matriz\", shape=box, style=filled, fillcolor=\"#AED6F1\"];\n\n";

    NodoFila* f = c->matriz->Fila1;
    string prevFilaId = matId;

    while (f != nullptr) {
        string filaId = "fila_" + to_string(idCapa) + "_" + to_string(f->Fila);
        dot += "  " + filaId + " [label=\"" + to_string(f->Fila) + "\", shape=box, style=filled, fillcolor=\"#D5F5E3\"];\n";
        dot += "  " + prevFilaId + " -> " + filaId + " [style=bold];\n";
        
        // CORRECCIÓN: Actualizamos el seguidor de filas para la jerarquía del dibujo
        prevFilaId = filaId; 

        NodoCol* col = f->Col1;
        string prevColId = filaId;
        while (col != nullptr) {
            string colId = "col_" + to_string(idCapa) + "_" + to_string(f->Fila) + "_" + to_string(col->Col);
            dot += "  " + colId + " [label=\"col:" + to_string(col->Col) + "\\n" + col->Color + "\", shape=box, style=filled, fillcolor=\"#FADBD8\"];\n";
            dot += "  " + prevColId + " -> " + colId + ";\n";
            prevColId = colId;
            col = col->SigC;
        }

        f = f->SigFila;
    }

    dot += "}\n";
    return dot;
}

//Graphviz para imagenes

string ABBNodosR(NodoABB* nodo) {
    if (nodo == nullptr) {
        return "";
    }
    string dot = "";
    dot += "    abb_" + to_string(nodo->Capa->ID) + " [label=\"capa_" + to_string(nodo->Capa->ID) + "\", shape=box, style=filled, fillcolor=\"#D2B4DE\"];\n";
    if (nodo->left) {
        dot += "    abb_" + to_string(nodo->Capa->ID) + " -> abb_" + to_string(nodo->left->Capa->ID) + ";\n";
        dot += ABBNodosR(nodo->left);
    }
    if (nodo->right) {
        dot += "    abb_" + to_string(nodo->Capa->ID) + " -> abb_" + to_string(nodo->right->Capa->ID) + ";\n";
        dot += ABBNodosR(nodo->right);
    }
    return dot;
}

string DotImg_ABB(Listas_img& lista, CapasABB& abb, int idImg) {
    Imagen* img = lista.Buscar(idImg);
    if (!img){
        return "// Imagen " + to_string(idImg) + " no encontrada\n";
    }
    string dot = "digraph ImagenABB_" + to_string(idImg) + " {\n";
    dot += "  rankdir=LR;\n";
    dot += "  node [fontname=\"Helvetica\"];\n\n";
    dot += "  subgraph cluster_imagen {\n";
    dot += "    label=\"Imagen " + to_string(idImg) + " - Lista de capas\";\n";
    dot += "    style=filled; fillcolor=\"#EBF5FB\";\n\n";

    string imgId = "img_" + to_string(idImg);
    dot += "    " + imgId + " [label=\"imagen" + to_string(idImg) + "\", shape=box, style=filled, fillcolor=\"#85C1E9\"];\n";

    Nodo_Capa* nc = img->capas;
    string prevId = imgId;
    int pos = 0;
    while (nc != nullptr) {
        string lcId = "lc_" + to_string(idImg) + "_" + to_string(pos);
        dot += "    " + lcId + " [label=\"capa_" + to_string(nc->capa->ID) + "\", shape=box, style=filled, fillcolor=\"#AED6F1\"];\n";
        dot += "    " + prevId + " -> " + lcId + ";\n";
        prevId = lcId;
        nc = nc->Sig;
        pos++;
    }
    dot += "  }\n\n";
    dot += "  subgraph cluster_abb {\n";
    dot += "    label=\"ABB de Capas\";\n";
    dot += "    style=filled; fillcolor=\"#F9EBEA\";\n\n";
    dot += ABBNodosR(abb.root);
    dot += "  }\n\n";
    dot += "  // enlaces lista -> ABB\n";
    nc = img->capas;
    pos = 0;
    while (nc != nullptr) {
        string lcId = "lc_" + to_string(idImg) + "_" + to_string(pos);
        string abbId = "abb_" + to_string(nc->capa->ID);
        dot += "  " + lcId + " -> " + abbId +
               " [style=dashed, color=red, constraint=false];\n";
        nc = nc->Sig;
        pos++;
    }
    dot += "}\n";
    return dot;
}

//Guardado

void GuardarDot(const string& nombre, const string& contenido) {
    ofstream f(nombre);
    f << contenido;
    f.close();
    cout << "Archivo DOT generado: " << nombre << endl;

    string nombrePng = nombre.substr(0, nombre.find_last_of('.')) + ".png";
    string comando = "dot -Tpng \"" + nombre + "\" -o \"" + nombrePng + "\"";
    system(comando.c_str());
    ifstream check(nombrePng);
    if (check.good())
        cout << "Imagen PNG generada: " << nombrePng << endl;
    else
        cout << "No se genero el PNG. Verifica que Graphviz este instalado y en el PATH.\n";
}

void SoloDot(const string& nombre, const string& contenido) {
    ofstream f(nombre);
    f << contenido;
    f.close();
    cout << "Archivo DOT generado: " << nombre << endl;
}

//Interfaz

int main(){
    CapasABB capasabb;
    Listas_img listasimg;
    ABBUsuarios usersabb;

    int op;
    do{
        cout <<"    MENU PRINCIPAL\n";
        cout <<"\n";
        cout <<"CARGA MASIVA\n";
        cout <<"1. Cargar capas\n";
        cout <<"2. Cargar imagenes\n";
        cout <<"3. Cargar Usuarios\n";
        cout <<"GENERACION DE IMAGENES\n";
        cout <<"4. Generar por recorrido de arboles\n";
        cout <<"5. Generar por ID de imagen\n";
        cout <<"6. Generar por ID de capa\n";
        cout <<"7. Generar por usuario\n";
        cout <<"CRUD DE USUARIOS\n";
        cout <<"8. Agregar usuario\n";
        cout <<"9. Eliminar usuario\n";
        cout <<"10. Modificar usuario\n";
        cout <<"CRUD IMAGENES\n";
        cout <<"11. Agregar imagen\n";
        cout <<"12. Eliminar imagen\n";
        cout <<"GRAFICAR O DIBUJAR\n";
        cout <<"13. Graficar ABB de capas\n";
        cout <<"14. Graficar lista circular de imagenes\n";
        cout <<"15. Graficar ABB de usuarios\n";
        cout <<"16. Graficar Matriz dispersa\n";
        cout <<"17. Graficar imagen junto ABB\n";
        cout <<"0. Salir\n";
        cout <<"Opcion: ";
        cin >> op;

        if(op == 1){
            string arch = ReadLine("Archivo .cap: ");
            Cargar_C(arch, capasabb);
        }
        else if(op == 2){
            string arch = ReadLine("Archivo .im: ");
            Cargar_I(arch, listasimg, capasabb);
        }
        else if(op == 3){
            string arch = ReadLine("Archivo .usr: ");
            Cargar_U(arch, usersabb, listasimg);
        }
        else if(op == 4){
            int n;
            string tipoR, salida;
            cout << "Numero de capas: ";
            cin >> n;
            cout << "Tipo de recorrido: ";
            cin >> tipoR;
            cout << "Nomre de archivo de salida: ";
            cin >> salida;
            Generacion_R(capasabb, n, tipoR, salida);
        }
        else if(op == 5){
            int id;
            string salida;
            cout << "ID imagen: ";
            cin >> id;
            cout << "Nombre de archivo: ";
            cin >> salida;
            Generar_I(listasimg, id, salida);
        }
        else if(op == 6){
            int id; 
            string salida;
            cout << "ID de capa: ";
            cin >> id;
            cout << "Nombre de archivo: ";
            cin >> salida;
            Generar_C(capasabb, id, salida);
        }
        else if(op == 7){
            string user, salida;
            int id_img;
            cout << "Nombre de usuario: ";
            cin >> user;
            Usuario* u = usersabb.Buscar(user);
            if(u){
                cout << "Imagenes de " << user <<": ";
                Nodo_ListaImg* list = u->Listaimg;
                while(list != nullptr){
                    cout << list->Img->ID <<" "; list = list->Sig;
                }
                cout << "\n";
            }
            cout << "ID de imagen: ";
            cin >> id_img;
            cout << "Nombre de archivo: ";
            cin >> salida;
            Generar_U(usersabb, listasimg, user, id_img, salida);
        }
        else if(op == 8){
            CRUDAgregar_U(usersabb);
        }
        else if(op == 9){
            CRUDEliminar_U(usersabb, listasimg);
        }
        else if(op == 10){
            CRUDModificar_U(usersabb);
        }
        else if(op == 11){
            CRUDAgregar_I(listasimg, capasabb, usersabb);
        }
        else if(op == 12){
            CRUDEliminar_I(listasimg, usersabb);
        }
        else if(op == 13){
            GuardarDot("ABB_capas.dot", capasabb.ToDot());
        }
        else if(op == 14){
            GuardarDot("lista_imagenes.dot", listasimg.ToDot());
        }
        else if(op == 15){
            GuardarDot("ABB_Usuarios.dot", usersabb.ToDot());
        }
        else if(op == 16){
            int id;
            cout << "ID de capa a graficar: ";
            cin >> id;
            CapaG* c = capasabb.Buscar(id);
            if(!c){
                cout << "Capa " << id << " no encontrada en el ABB.\n";
            } else {
                string archivo = "capa_" + to_string(id) + ".dot";
                SoloDot(archivo, dotCapaIndividual(capasabb, id));
            }
        }
        else if(op == 17){
            int id_img; 
            cout << "ID de imagen: ";
            cin >> id_img;
            string archivo = "imagen_abb_" + to_string(id_img) + ".dot";
            GuardarDot(archivo, DotImg_ABB(listasimg, capasabb, id_img));
        }
    }while(op != 0);
    return 0;
}