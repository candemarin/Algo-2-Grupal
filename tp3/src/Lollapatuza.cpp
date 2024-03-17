//
// Created by juanb on 28/06/23.
//
#include "Lollapatuza.h"

Lollapatuza::Lollapatuza() {
    // Copio los argumentos para evitar Aliasing e inicializo las estructuras
    this->_diccPuestos = map<Nat,PuestoDeComida>();
    this->_conjPersona = set<Persona>();
    this->_listaPuestos = list<tuple<Nat,PuestoDeComida>>();
    this->_diccPersonas = map<Persona, Nat>();
}

Lollapatuza::Lollapatuza(const map<Nat,PuestoDeComida>& puestos, const set<Persona>& personas) {
    // Copio los argumentos para evitar Aliasing e inicializo las estructuras
    this->_diccPuestos = puestos;
    this->_conjPersona = personas;
    this->_listaPuestos = list<tuple<Nat,PuestoDeComida>>();

    // Agrego los Puestos a listaPuestos
    auto itPuesto = this->_diccPuestos.begin();
    while (itPuesto != this->_diccPuestos.end()){
        this->_listaPuestos.push_back(make_pair(itPuesto->first,itPuesto->second));
        ++itPuesto;
    }

    // Inicializo los gastos de todas las personas a 0;
    this->_diccPersonas = map<Persona, Nat>();
    auto itPersona = this->_conjPersona.begin();
    while(itPersona != this->_conjPersona.end()){
        this->_colaPrioridadGastos.insert(make_pair(make_tuple(0,*itPersona), *itPersona));
        this->_diccPersonas.insert(make_pair(*itPersona,0));
        ++itPersona;
    }
}

const set<Persona> &Lollapatuza::personas() const {
    return this->_conjPersona;
}

const list<tuple<Nat, PuestoDeComida>>& Lollapatuza::puestos() const {
    return this->_listaPuestos;
}

const PuestoDeComida& Lollapatuza::getPuestoDeComida(const IdPuesto puestoId) const {
    return this->_diccPuestos.at(puestoId);
}

void Lollapatuza::vender(const IdPuesto puestoId, const Persona per, const Producto item, const Nat cant) {
    //guardo en una variable por referencia al puesto que vendio, para tenerlo a mano.
    PuestoDeComida& puestoVendedor = this->_diccPuestos.at(puestoId);

    // Elimino la clave anterior de la cola de prioridad
    Nat& gastoPersona = this->_diccPersonas.at(per);
    this->_colaPrioridadGastos.erase(make_tuple(gastoPersona,per));

    //Registro la venta en el puesto.
    puestoVendedor.registrarVenta(item,per,cant);

    // Si la venta fue hackeable, guardo el puesto de comida como opción a hackear
    if(puestoVendedor.ventaSinPromo(item,cant)){

        auto itDiccInfoHackeoItem = this->_diccInfoHackeo.find(per);

        // Defino los diccionarios necesarios si aun no lo estaban,
        // el de la persona, el del items y finalmente el de posibles puestos para hackear
        if(itDiccInfoHackeoItem == this->_diccInfoHackeo.end()) {
            map<Producto,map<Nat,PuestoDeComida*>> DiccInfoHackeoItem = map<Producto,map<Nat,PuestoDeComida*>>();
            itDiccInfoHackeoItem = this->_diccInfoHackeo.insert(make_pair(per, DiccInfoHackeoItem)).first;
        }
        auto itDiccPuestosHackeos = itDiccInfoHackeoItem->second.find(item);

        if(itDiccPuestosHackeos == itDiccInfoHackeoItem->second.end()) {
            map<Nat,PuestoDeComida*> DiccPuestosHackeos = map<Nat,PuestoDeComida*>();
            itDiccPuestosHackeos = itDiccInfoHackeoItem->second.insert(make_pair(item, DiccPuestosHackeos)).first;
        }

        map<Nat,PuestoDeComida*>& DiccPuestosHackeos = itDiccPuestosHackeos->second;

        // Si el puesto ya había sido guardado como una opción, no se hace nada. En caso contrario, agrego su puntero
        // y uso su ID como Clave
        // Esto es para encontrar rapidamente el puesto de menor ID, gracias a la implementacion por AVL.
        if(DiccPuestosHackeos.count(puestoId) == 0){
            DiccPuestosHackeos.insert(make_pair(puestoId,&puestoVendedor));
        }

    }

    //Actualizo el gasto de la persona
    Nat costoVenta = puestoVendedor.calcularCostoVenta(item,cant);
    gastoPersona += costoVenta;

    //Agrego de nuevo el gasto de la persona en la cola de prioridad.
    tuple<Nat,Persona> infoGasto = make_tuple(gastoPersona,per);
    this->_colaPrioridadGastos.insert(make_pair(infoGasto,per));

}

void Lollapatuza::hackear(const Persona per, const Producto item) {
    // Obtengo los puestos que pueden ser hackeados con la persona e item dados
    map<Producto,map<Nat,PuestoDeComida*>>& diccInfoHackeoItem = this->_diccInfoHackeo.at(per);
    map<Nat,PuestoDeComida*>& diccPuestosHackeo = diccInfoHackeoItem.at(item);

    //Agarro el primer Iterador del Arbol AVL, al ser en InOrder debería ser extremo izquierdo, y por eso el mínimo.
    auto itIDMenor = diccPuestosHackeo.begin();
    //Utilizamos punteros en vez de iteradores por comodidad. Tambien tenia mas sentido utilizar punteros.
    PuestoDeComida* puestoMenorId = itIDMenor->second;

    //Elimino el gasto desactualizado del puesto de Comida
    Nat& gastoPersona = this->_diccPersonas.at(per);
    this->_colaPrioridadGastos.erase(make_tuple(gastoPersona,per));

    //Le resto el gasto de la persona en dicho puesto al gasto total de la misma, ya que después del hackeo esté se le
    //sumará el actualizado.
    gastoPersona -= puestoMenorId->gastosDe(per);

    // Corro el Hackeo del puesto
    puestoMenorId->hackeo(item,per);

    // Si sigue siendo hackeable el puesto no hacemos nada. En el caso contrario lo eliminamos del diccionario.
    if(!puestoMenorId->esHackeable(item,per)){
        diccPuestosHackeo.erase(itIDMenor->first);
    }

    // Actualizo el gasto de la persona y lo agrego a la cola de prioridad
    gastoPersona += puestoMenorId->gastosDe(per);
    this->_colaPrioridadGastos.insert(make_pair(make_tuple(gastoPersona,per), per));
}


Nat Lollapatuza::gastoTotalDe(const Persona persona) const {
    if (this->_diccPersonas.count(persona) == 1){
        return this->_diccPersonas.at(persona);
    }
    return 0;
}

IdPuesto Lollapatuza::menorStock(const Producto item) const {
    // Obtengo el ultimo iterador del diccionario con end
    auto itDiccionario = this->_diccPuestos.end();

    // Obtengo el ID del puesto que esta siendo apuntado
    IdPuesto puestoMinId = itDiccionario->first;
    Nat MinStock = 0;
    bool StockEncontrado = false;

    // Hago una bandera que apunte al primer iterador del diccionario
    auto primerIt = this->_diccPuestos.begin();

    while (itDiccionario != this->_diccPuestos.begin()){
        //Retrocedo voy al iterador anterior
        --itDiccionario;

        //Obtengo el puesto actual
        const PuestoDeComida& puestoActual = itDiccionario->second;

        // Mientras que no encuentre un puesto que tenga al item en su lista de stock,
        // Reemplazamos automaticamente el puestoMinId al siguiente puesto
        if (!StockEncontrado) {
            puestoMinId = itDiccionario->first;

            // Verificamos si el puesto contiene al item en su lista de Stock
            if(puestoActual.productoEnVenta(item)){

                // Si es así, modificamos la flag StockEncontrado y actualizo el mínimo stock encontrado.
                MinStock = puestoActual.obtenerStock(item);
                StockEncontrado = true;
            }
        // Si ya se encontro un puesto con el item en el stock, lo comparamos con el puesto actual para ver cual es menor.
        }else if(puestoActual.productoEnVenta(item)){
            int StockActual = puestoActual.obtenerStock(item);

            // Si tienen el mismo stock, ganará el puesto actual ya que debe entregar el puesto con menor ID.
            // Al recorrerse en InOrder, se que el ID Actual será menor a todos los anteriores
            if (StockActual <= MinStock){

                // Reemplazamos el valor del mínimo id por el valor actual y también el stock.
                puestoMinId = itDiccionario->first;
                MinStock = StockActual;
            }
        }
    }
    return puestoMinId;
}

Persona Lollapatuza::personaMasGasto() const {
    auto personaMasGasto = this->_colaPrioridadGastos.end();
    --personaMasGasto;
    return personaMasGasto->second;
}