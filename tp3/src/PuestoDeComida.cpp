//
// Created by juanb on 26/06/23.
//
#include "PuestoDeComida.h"

PuestoDeComida::PuestoDeComida(aed2_Puesto argsPuesto) {
    // Guardo tanto el menu como el stock.
    this->_diccPrecio = argsPuesto.menu;
    this->_diccStock = argsPuesto.stock;

    // Inicializo diccVentas, diccHackeos y diccDescuentos.
    this->_diccVentas = map<Persona, registroVentasPersona>();
    this->_diccHackeos = map<Persona, map<Producto, list<IteradorVenta>>>();
    this->_diccDescuentos = map<Producto, InfoDescuentos>();

    // Construimos los vectores de Descuentos por cada item
    Promociones::iterator itPromociones = argsPuesto.promociones.begin();
    while(itPromociones != argsPuesto.promociones.end()){

        // Creamos un vector vacío en donde asociaremos cada posición con su cantidad y su descuento correspondiente.
        vector<Nat> vectorDescuentos = vector<Nat>();
        map<Nat, Nat>& diccCantDescuentos = itPromociones->second;
        map<Nat, Nat>::iterator itCantDescuento = diccCantDescuentos.begin();
        int indexActual = 0;
        Nat DescuentoActual = 0;

        // Iteramos por cada cantidad de item en donde el descuento aumenta
        while(itCantDescuento != diccCantDescuentos.end()){

            // Mientras nuestro index sea menor a la cantidad en donde el descuento aumenta, seguimos agregando
            // El porcentaje anterior (el cual incialmente es 0)
            while(indexActual < itCantDescuento->first){
                vectorDescuentos.push_back(DescuentoActual);
                indexActual++;
            }

            //Al llegar a una cantidad con un nuevo descuento, empezamos a registrar este.
            DescuentoActual = itCantDescuento->second;
            ++itCantDescuento;
        }

        // Dado que el ciclo parará antes de agregar el último descuento, lo colocamos de forma manual y guardamos su
        // Cantidad minima, esta sera igual al indexAcutal, ya que nos detuvimos cuando este se volvio la ultima cantidad
        vectorDescuentos.push_back(DescuentoActual);
        Nat maxCantidad = indexActual;

        // Asociamos la información del descuento con el item
        InfoDescuentos infoDescuento = InfoDescuentos(maxCantidad, vectorDescuentos);
        this->_diccDescuentos.insert(pair<Producto,InfoDescuentos>(itPromociones->first, infoDescuento));
        ++itPromociones;
    }

}

PuestoDeComida::PuestoDeComida(const PuestoDeComida& pc) { *this = pc; }

PuestoDeComida& PuestoDeComida::operator=(const PuestoDeComida& pc) {
    //Copio las estructuras de Puesto que no utilizan iteradores
    this->_diccPrecio = pc._diccPrecio;
    this->_diccStock = pc._diccStock;
    this->_diccDescuentos = pc._diccDescuentos;
    this->_diccVentas = pc._diccVentas;

    //Inicializo diccHackeos para guardar a los iteradores propios
    this->_diccHackeos = map<Persona, map<Producto, list<IteradorVenta>>>();

    // Por cada persona y por cada uno de sus items comprados busco las ventas hackeables originales y creo los nuevos
    //iteradores apuntando a las copias de las ventas.
    auto itHackeos = pc._diccHackeos.begin();
    while(itHackeos != pc._diccHackeos.end()){
        //Guardo la persona con ventas hackeables
        const Persona& personaHackeada = itHackeos->first;
        map<Producto, list<IteradorVenta>> DiccItemsHackeoCopia = map<Producto, list<IteradorVenta>>();

        // Por cada item que tiene la posibilidad de ser hackeable creo los nuevos iteradores que apuntan a las copias de
        // Ventas
        auto itItemsHackeo = itHackeos->second.begin();
        while(itItemsHackeo != itHackeos->second.end()){
            //Guardo el item con ventas hackeables
            const Producto& itemHackeado = itItemsHackeo->first;

            //Inicializo la lista para guardar los iteradores
            list<IteradorVenta> ListaVentasHackCopia = list<IteradorVenta>();

            //Me preparo para recorrer los iteradores a ventas Hackeables y el propio historial
            auto itListaVentasHackeables = itItemsHackeo->second.begin();
            const list<tuple<Producto, Nat>>& historial = this->_diccVentas.at(personaHackeada).historial;
            auto itListaVentas = historial.begin();

            // Recorro las ventas copiadas y guardo los iteradores de las cuales se encontraban originalmente en la
            // lista de VentasHackeables
            // Tanto el historial como la lista de iteradores estan ordenadas cronologicamente de la misma manera.
            // Asi que puedo recorrer el historial una sola vez
            while(itListaVentasHackeables != itItemsHackeo->second.end()){
                tuple<Producto, Nat> ventaHackeable = *(*itListaVentasHackeables);
                tuple<Producto, Nat> ventaActual = *(itListaVentas);
                //Si encuentro la venta, avanzo tanto la lista de ventas Hackeables como el historial y guardo el historial
                //Sino, solo el historial
                if(ventaActual == ventaHackeable){
                    ListaVentasHackCopia.push_back(itListaVentas);
                    ++itListaVentasHackeables;
                }
                ++itListaVentas;
            }
            //relaciono la lista con el item hackeado.
            DiccItemsHackeoCopia.insert(pair<Producto ,list<IteradorVenta>>(itemHackeado, ListaVentasHackCopia));
            ++itItemsHackeo;
        }
        //relaciono las ventas hackeables con la persona.
        this->_diccHackeos.insert(pair<Persona ,map<Producto, list<IteradorVenta>>>(personaHackeada, DiccItemsHackeoCopia));
        ++itHackeos;
    }

}

const Nat PuestoDeComida::descuento(const Producto item, const Nat cant) const {
    // Verifico que el item tenga algun descuento registrado
    if(this->_diccDescuentos.count(item) > 0){

        // DescuentoInfo.listaDescuento tiene guardado en cada posición el porcentaje de descuento
        // Asignado a esa cantidad
        // Si la cantidad es mayor al máximo, devolvemos el descuento más grande
        const InfoDescuentos& infoDescuento = this->_diccDescuentos.at(item);
        if (cant > infoDescuento._maxCantidad){
            return infoDescuento._listaDescuento[infoDescuento._maxCantidad];
        }else{
            return infoDescuento._listaDescuento[cant];
        }
    }
    return 0;
}

const Nat PuestoDeComida::calcularCostoVenta(const Producto item, const Nat cant) const {
    Nat precioUnit = this->_diccPrecio.at(item);
    Nat descProd = descuento(item, cant);
    Nat res = cant * (precioUnit * (100 - descProd) / 100);
    return res;
}

const bool PuestoDeComida::ventaSinPromo(const Producto item, const Nat cant) const {
    Nat descProd = descuento(item, cant);
    if (descProd == 0) {
        return true;
    }
    return false;
}

void PuestoDeComida::registrarVenta(const Producto item, const Persona persona, const Nat cantidad) {
    Nat* stockDisponible = &this->_diccStock.at(item);

    // Solo se efectua la venta si hay suficiente cantidad
    if(*stockDisponible >= cantidad) {
        // Reduzco la cantidad de stock disponible
        *stockDisponible -= cantidad;

        // Calculo el precio de venta, y se lo agrego al historial de compras de la persona
        Nat precioVenta = calcularCostoVenta(item, cantidad);
        auto itVentas = this->_diccVentas.find(persona);

        // Si la persona no tiene compras hechas en el puesto la agrego al historial del mismo
        if(itVentas == this->_diccVentas.end()) {
            registroVentasPersona registroNuevaPersona;
            registroNuevaPersona.total = 0;
            itVentas = this->_diccVentas.insert(make_pair(persona, registroNuevaPersona)).first;
        }

        registroVentasPersona& historialCliente = itVentas->second;
        historialCliente.total += precioVenta;
        historialCliente.historial.insert(historialCliente.historial.end(), make_tuple(item, cantidad));

        // Si la venta es hackeable, nos guardamos el iterador a su posicion
        if(ventaSinPromo(item, cantidad)) {
            // La venta es hackeable. Me guardo el iterador
            IteradorVenta iteradorDeVentaHackeable = historialCliente.historial.end();
            --iteradorDeVentaHackeable;

            // De ser la primer compra hackeable de la persona, creamos su historial de compras hackeables
            auto itHistorialPersona = this->_diccHackeos.find(persona);
            if(itHistorialPersona == this->_diccHackeos.end()) {
                map<Producto, list<IteradorVenta>> nuevoHistorialDeCompras;
                itHistorialPersona = this->_diccHackeos.insert(make_pair(persona, nuevoHistorialDeCompras)).first;
            }

            // Si la persona nunca hizo una compra hackeable del producto, lo agregamos a la lista de sus compras hackeables
            auto itProductoHackeable = itHistorialPersona->second.find(item);
            if(itProductoHackeable == itHistorialPersona->second.end()) {
                list<IteradorVenta> nuevaListaDeCompras;
                itProductoHackeable = itHistorialPersona->second.insert(make_pair(item, nuevaListaDeCompras)).first;
            }

            // Agregamos la compra de este producto al historial de hackeables con su iterador
            itProductoHackeable->second.insert(itProductoHackeable->second.end(), iteradorDeVentaHackeable);
        }
    }
    return;
}

const Nat PuestoDeComida::gastosDe(const Persona persona) const{
    if (this->_diccVentas.count(persona) == 1){
        return this->_diccVentas.at(persona).total;
    }
    return 0;
};

bool PuestoDeComida::esHackeable(const Producto item, const Persona persona) {
    map<Producto, list<IteradorVenta>>& diccHackeosItems = this->_diccHackeos[persona];
    list<IteradorVenta> listaVentasHackeables = diccHackeosItems[item];
    return listaVentasHackeables.size() != 0;
}

const bool PuestoDeComida::productoEnVenta(const Producto item) const{
    return  this->_diccPrecio.count(item) > 0;
}

void PuestoDeComida::hackeo(const Producto item, const Persona a) {
    map<Producto, list<IteradorVenta>>& diccHackeosItems = this->_diccHackeos[a];
    list<IteradorVenta>& listaVentasHackeables = diccHackeosItems[item];

    // Obtengo el iterador apuntado a la primera venta hackeable y guardo en itemHackeado y cantHackeado su información
    _List_iterator<IteradorVenta> itVentaHackeable = listaVentasHackeables.begin();
    IteradorVenta ventaHackeable = *itVentaHackeable;
    Producto itemHackeado = get<0>(*ventaHackeable);
    Nat cantHackeado = get<1>(*ventaHackeable);

    // Actualizo el Stock, agregándole la cantidad hackeada.
    this-> _diccStock[itemHackeado] += cantHackeado;

    // Actualizo el registroVentasPersona, restándole el gasto de la venta Hackeada y borrando el item del historial
    Nat precioVenta = calcularCostoVenta(itemHackeado, cantHackeado);
    this-> _diccVentas[a].total -= precioVenta;
    this-> _diccVentas[a].historial.erase(ventaHackeable);

    // Elimino la venta del historial y su iterador.
    listaVentasHackeables.erase(itVentaHackeable);
}

const Nat PuestoDeComida::obtenerStock(const Producto item) const{
    return this->_diccStock.at(item);
}