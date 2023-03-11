//
// Assignment 2 DPST1091: CS Pizzeria
// pizzeria.c
//
// This program was written by YOUR-NAME-HERE (z5555555)
// on INSERT-DATE-HERE
//
// TODO: INSERT-DESCRIPTION-HERE
//
// Version 1.0.0: Release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: add any extra #includes your code needs here.

#include "pizzeria.h"

// TODO: add your own #defines here.

struct ingredient {
    // TODO: add your own fields in Stage 2.
    char *name[MAX_STR_LENGTH];
    double price;
    int amount;
    struct ingredient *next;
};

struct order {
    // TODO: add your own fields in Stage 1.
    // Looking to store a price field? Try putting in "double price;" here!
    struct order *next;
    char name[MAX_STR_LENGTH];
    char pizza[MAX_STR_LENGTH];
    double price;
    int time;
    struct ingredient *ingredients;
};

struct pizzeria {
    // TODO: add your own fields if required.
    struct order *orders;
    struct order *selected;
};

// TODO: add any other structs you define here.

//////////////////////////////////////////////////////////////////////

// TODO: add prototypes for any helper functions you create here.

struct order* create_order( 
    char *customer,
    char *pizze_name,
    double price,
    int time_allowed
);

// Prints a single order
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
);

// Prints an ingredient given the name, amount and price in the required format.
// This will be needed for stage 2.
void print_ingredient(char *name, int amount, double price);

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////


struct pizzeria *create_pizzeria() {
    // Allocates memory to store a `struct pizzeria` and returns a pointer to
    // it. The variable `new` holds this pointer!
    struct pizzeria *new = malloc(sizeof(struct pizzeria));
    new->orders = NULL;
    new->selected = NULL;

    // TODO: this function has already been implemented for the current
    // struct pizzeria. When you decide to change struct pizzeria, change
    // this function as well.

    return new;
}

int add_order(
    struct pizzeria *pizzeria,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
) {

    // TODO: Stage 1.1.1: add_order function

    if ( price <= 0 ) {

        return INVALID_PRICE;

    } else if ( time_allowed < 0 ) {

        return INVALID_TIME;

    }
    
    struct order *new = create_order(customer, pizza_name, price, time_allowed);

    if ( pizzeria->orders == NULL ) {

        pizzeria->orders = new;

    } else {

        struct order *i = pizzeria->orders;
        while ( i->next != NULL ) {

            i = i->next;

        }
        i->next = new;
    }

    return SUCCESS;
}

void print_all_orders(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    int count = 1;
    for ( struct order *item = pizzeria->orders; item != NULL; item = item->next ) {

        print_order(count, item->name, item->pizza, item->price, item->time);
        count = count + 1;

    }

    // Do not change this part of the function
    print_selected_order(pizzeria);
    return;
}

int next_deadline(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function
    if ( pizzeria->orders == NULL ) {

        return INVALID_CALL;
    }

    int deadline = pizzeria->orders->time;

    struct order *i = pizzeria->orders->next;

    //printf("Deadline: %d\n", deadline);
    while ( i != NULL ) {

        if ( deadline > i->time ) {

            deadline = i->time;
            //printf("Deadline: %d\n", deadline);

        }

        i = i->next;
    }

    return deadline;
}

// #Cmd lines for test till stage 1.2#
// o Harry Margarita 15 20
// o Dumbledore Beef-n-Onion 10 30
// o Molly Pepperoni 12.5 30
// o Remus Nutella 18.99 50
// o Percy Anchovies 19.99 10
// o Hermione Grilled-Vegetable 14.49 60

// o Harry Margarita 15 20
// o Hermione Grilled-Vegetable 14.49 60
// o Molly Pepperoni 12.5 30
// o Percy Anchovies 19.99 50
// o Dumbledore Beef-n-Onion 10 30
// o Remus Nutella 18.99 50
// !
// q

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////


void select_next_order(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return;
}

void select_previous_order(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return;
}

void print_selected_order(struct pizzeria *pizzeria) {

    // TODO: Change this once you have implemented the selected order
    // functionality.

    // apostrophe needs'\' in the printf().

    printf("\nNo selected order.\n");
}

int add_ingredient(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {

    // TODO: fill in the rest of this function

    return SUCCESS;
}

double calculate_total_profit(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return 0.0;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////


int cancel_order(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return SUCCESS;
}

void free_pizzeria(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return;
}

int refill_stock(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {

    // TODO: fill in the rest of this function

    return SUCCESS;
}

void print_stock(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return;
}

int can_complete_order(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

int complete_order(struct pizzeria *pizzeria) {

    return SUCCESS;
}

int save_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

int load_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//               HELPER FUNCTIONS - Add your own here                 //
////////////////////////////////////////////////////////////////////////

// Prints a single order
//
// `print_order` will be given the parameters:
// - `num` -- the integer that represents which order it is sequentially.
// - `customer` -- the name of the customer for that order.
// - `pizza_name` -- the pizza the customer ordered.
// - `price` -- the price the customer is paying for the pizza.
// - `time_allowed` -- the time the customer will wait for the order.
//
// `print_order` assumes all parameters are valid.
//
// `print_order` returns nothing.
//
// This will be needed for Stage 1.
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,

    int time_allowed
) {

    printf("%02d: %s ordered a %s pizza ($%.2lf) due in %d minutes.\n",
        num, customer, pizza_name, price, time_allowed);

    return;
}

// Prints a single ingredient
//
// `print_ingredient` will be given the parameters:
// - `name` -- the string which contains the ingredient's name.
// - `amount` -- how many of the ingredient we either need or have.
// - `price` -- the price the ingredient costs.
//
// `print_ingredient` assumes all parameters are valid.
//
// `print_ingredient` returns nothing.
//
// This will be needed for Stage 2.
void print_ingredient(char *name, int amount, double price) {
    printf("    %s: %d @ $%.2lf\n", name, amount, price);
}

struct order* create_order( 
    char *customer,
    char *pizze_name,
    double price,
    int time_allowed
){

    struct order *new = malloc(sizeof(struct order));

    strcpy(new->name, customer);
    strcpy(new->pizza, pizze_name);
    new->price = price;
    new->time = time_allowed;
    new->next = NULL;
    new->ingredients = NULL;

    return new;
}

