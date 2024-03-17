//
// Created by juanb on 26/06/23.
//

#ifndef TP_LOLLA_PUESTODECOMIDA_H
#define TP_LOLLA_PUESTODECOMIDA_H
#include <set>
#include <vector>
#include <list>
#include "tipos.h"

class PuestoDeComida {
public:
    // Genera un nuevo puesto de comida dado un menu, una lista de descuentos y un stock
    PuestoDeComida(const aed2_Puesto);

    // Genera un puesto de comida por referencia
    PuestoDeComida(const PuestoDeComida&);

    // Genera por copia un puesto de comida
    PuestoDeComida& operator=(const PuestoDeComida&) ;

    // Indica la cantidad de descuento a aplicar en un producto
    const Nat descuento(const Producto, const Nat) const;

    // Indica el costo de venta de un producto dada una cantidad
    const Nat calcularCostoVenta(const Producto, const Nat) const;

    // Determina si la venta tiene un descuento aplicable
    const bool ventaSinPromo(const Producto, const Nat) const;

    // Registra la venta del item y la cantidad del mismo
    void registrarVenta(const Producto, const Persona, const Nat);

    // Devuelve la cantidad de stock que hay de un producto
    const Nat obtenerStock(const Producto) const;

    // Determina si esta en el menu el producto indicado
    const bool productoEnVenta(const Producto) const;

    // Informa el gasto del cliente
    const Nat gastosDe(const Persona) const;

    // Elimina del historial de la persona una venta hackeable (sin promocion aplicada)
    // Nuestra implementacion elimina la venta completa y no solo una unidad de ella
    // Esto es porque asi lo establecimos en nuestro TP2, y nos mantuvimos lo mas fiel posible a el.
    void hackeo(const Producto, const Persona);

    // Determina si se puede hackear la venta de un producto hecha por la persona indicada
    bool esHackeable(const Producto, const Persona);

private:
    struct InfoDescuentos{
        InfoDescuentos(const Nat maxCantidad, const vector<Nat> listaDescuento): _maxCantidad(maxCantidad), _listaDescuento(listaDescuento){};

        // maxCantidad : Es un natural que representa la cantidad mínima a comprar de un item para conseguir el mayor porcentaje
        // de descuento en dicho puesto
        Nat _maxCantidad;

        // listaDescuento: Es un vector donde cada posición representa una cantidad a comprar del item y devuelve el
        // procentaje de descuento apropiado. Su longitud es igual a la cantidad mínima a comprar de un item para conseguir
        // el mayor descuento + 1.
        vector<Nat> _listaDescuento;
    };

    struct registroVentasPersona{
        // total : Es un natural que representa el gasto total realizado por una persona en el puesto.
        Nat total;

        // historial : Es una lista que representa el historial de venta de una persona. Contiene todas las ventas en forma de
        // tupla, con su respectivo item y cantidad.
        list<tuple<Producto,Nat>> historial;
    };

    using IteradorVenta = list<tuple<Producto,Nat>>::const_iterator;

    // diccPrecio: Es un diccionario logarítmico que representa al menú del puesto. Éste asocia a cada item con su precio.
    Menu _diccPrecio;

    // diccStock : Es un diccionario logarítmico que representa al stock del puesto.
    // Éste asocia a cada item con su stock actual.
    Stock _diccStock;

    // diccDescuentos: Es un diccionario logarítmico que representa a los descuentos del puesto.
    // Éste asocia a cada item con descuento con los diferentes porcentajes que podría tener según su cantidad.
    map<Producto, InfoDescuentos> _diccDescuentos;

    // diccVentas: Es un diccionario logarítmico que asocia a las personas con su registro de venta.
    map<Persona, registroVentasPersona> _diccVentas;

    // diccHackeos: Es un diccionario logarítmico que relaciona a una persona con todas las ventas hackeables de un item específico.
    // La secuencia se utiliza para guardar iteradores apuntando a las ventas hackeables dentro de diccVentas.
    map<Persona, map<Producto, list<IteradorVenta>>> _diccHackeos;

};


#endif //TP_LOLLA_PUESTODECOMIDA_H
