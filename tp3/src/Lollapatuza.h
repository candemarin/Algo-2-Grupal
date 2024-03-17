//
// Created by juanb on 28/06/23.
//

#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H
#include <set>
#include <vector>
#include <list>
#include "tipos.h"
#include "PuestoDeComida.h"

class Lollapatuza {
public:

    Lollapatuza();
    Lollapatuza(const map<Nat,PuestoDeComida>&, const set<Persona>&);

    // Devuelve un conjunto con todas las personas
    const set<Persona>& personas() const;

    // Devuelve un conjunto con todos los ids de puestos
    const list<tuple<Nat, PuestoDeComida>>& puestos() const;

    // Registra una venta de un producto en un puesto
    void vender(const IdPuesto, const Persona, const Producto, const Nat);

    // Devuelve el gasto de una persona
    Nat gastoTotalDe(const Persona) const;

    // Hackea a persona. En nuestra implementacion el hackeo elimina la venta entera del puesto, no una sola unidad
    // Esto diferencia con el TP Original viene de que nuestro diseño tiene este comportamiento, por lo que decidimos ser fiel a el.
    void hackear(const Persona, const Producto);

    // Devuelve el puesto con menor stock de algun producto dado
    IdPuesto menorStock(const Producto) const;

    // Devuelve la persona que más gastó en el lolla
    Persona personaMasGasto() const;

    // Devuelve una referencia al objeto PuestoDeComida indicado por el ID
    const PuestoDeComida& getPuestoDeComida(const IdPuesto) const;

private:

    map<Nat,PuestoDeComida> _diccPuestos;
    list<tuple<Nat,PuestoDeComida>> _listaPuestos;
    map<Persona, Nat> _diccPersonas;
    set<Persona> _conjPersona;
    map<tuple<Nat,Persona>,Persona> _colaPrioridadGastos;
    map<Persona, map<Producto,map<Nat,PuestoDeComida*>>> _diccInfoHackeo;

};

#endif //TP_LOLLA_LOLLAPATUZA_H
