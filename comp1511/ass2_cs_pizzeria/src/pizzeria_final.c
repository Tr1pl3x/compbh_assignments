//
// Assignment 2 DPST1091: CS Pizzeria
// pizzeria.c
//
// This program was written by YOUR-NAME-HERE (z5271704)
// on 11 March 2022
//
// TODO: INSERT-DESCRIPTION-HERE
//
// Version 1.0.0: Release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: add any extra #includes your code needs here.

#include "pizzeria.h"
#include "save_string.h"

// TODO: add your own #defines here.

#define MAX_LINE 500

struct ingredient {
    // TODO: add your own fields in Stage 2.
    char name[MAX_STR_LENGTH];
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
    struct ingredient *stock;
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

// create a node for a new ingredient
struct ingredient *create_ingredient(
    char *ingredient_name, 
    int amount, 
    double price
);

// Inserts a new node into a linked list while keeping the list alphabetically ordered.
struct ingredient *add_ingredient_ordered(
    struct ingredient *new,
    struct ingredient *head
);

// Finds the ingredients in the stock
struct ingredient *find_stock(
    struct ingredient *head,
    char *stock_name
);

// Removes the required amount from the stock to complete the order
struct pizzeria* remove_stock(
    struct ingredient *stock,
    struct pizzeria *pizzeria
);

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////


struct pizzeria *create_pizzeria() {
    // Allocates memory to store a `struct pizzeria` and returns a pointer to
    // it. The variable `new` holds this pointer!
    struct pizzeria *new = malloc(sizeof(struct pizzeria));
    new->orders = NULL;
    new->selected = NULL;
    new->stock = NULL;
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


////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////


void select_next_order(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function
    if ( pizzeria->selected !=  NULL) {

        pizzeria->selected = pizzeria->selected->next;

    } else {

        //if nothing is selected, then the first node will be pointed as the selected pointer.

            if ( pizzeria->orders != NULL ) {
                pizzeria->selected = pizzeria->orders;
            }

    }

    return;
}

// selects the head again after going end of the list.

void select_previous_order(struct pizzeria *pizzeria) {
    // conditon = end of the node / selected node / selected is the first node
    // TODO: fill in the rest of this function
    struct order *i = pizzeria->orders;
    if ( pizzeria->selected == pizzeria->orders) {

        i = NULL;

    } else if ( pizzeria->selected != NULL) {

        while ( i->next != pizzeria->selected) {

            i = i->next;
        }

    } else {

        while ( i->next != NULL) {

            i = i->next;
        }

    }
    
    pizzeria->selected = i;

    return;
}

// cannot go previous of first node

void print_selected_order(struct pizzeria *pizzeria) {

    // TODO: Change this once you have implemented the selected order
    // functionality.

    // apostrophe needs'\' in the printf().
    if ( pizzeria->selected != NULL ) {

        printf("\nThe selected order is %s's %s pizza ($%.2lf) due in %d minutes.\n", pizzeria->selected->name, pizzeria->selected->pizza, pizzeria->selected->price, pizzeria->selected->time);
        
        for ( struct ingredient *i = pizzeria->selected->ingredients; i != NULL; i = i->next) {

            print_ingredient(i->name,i->amount, i->price);

        }

    } else { 

        printf("\nNo selected order.\n");
    }

}

int add_ingredient(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {

    // TODO: fill in the rest of this function

    if ( price < 0 ) {

        return INVALID_PRICE;

    }
    if ( amount <= 0 ) {

        return INVALID_AMOUNT;

    } 
    if ( pizzeria->selected == NULL ) {

        return INVALID_ORDER;

    }
    
    struct ingredient *i = pizzeria->selected->ingredients;
    int flag = 0;
    while ( i != NULL && flag == 0) {

        if ( strcmp( ingredient_name, i->name) == 0 ) {

            i->amount = i->amount + amount;
            flag = 1;

        } else {

            i = i->next;

        }

    }

    if ( i == NULL ) {
            
        struct ingredient *new_ingredient = create_ingredient(ingredient_name, amount, price);
        pizzeria->selected->ingredients = add_ingredient_ordered(new_ingredient, pizzeria->selected->ingredients);

    }


    return SUCCESS;
}



double calculate_total_profit(struct pizzeria *pizzeria) {

    // TODO: fill in the rest of this function
    if ( pizzeria->selected == NULL ) {

        return INVALID_ORDER;
    }

    struct ingredient *i = pizzeria->selected->ingredients;


    double sum = 0;
    while ( i != NULL ) {

        sum = sum + ( i->amount * i->price);
        i = i->next;
    }

    return pizzeria->selected->price - sum;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////


int cancel_order(struct pizzeria *pizzeria) {

    if ( pizzeria->selected == NULL ) {

        return INVALID_ORDER;

    }

    struct order* cancel = pizzeria->selected;

    if ( cancel->ingredients != NULL ) {

        struct ingredient *i = cancel->ingredients;

        while ( i != NULL ) {

            struct ingredient *temp = i->next;
            free(i);
            i = temp;

        }

        cancel->ingredients = i;
    }

    struct order *i = pizzeria->orders;

    if ( i == cancel ) {

        i = i->next;
        free(cancel);
        pizzeria->orders = i;
        pizzeria->selected = i;

    } else {

        while ( i->next != cancel ) {

            i = i->next;
        }

        i->next = cancel->next;
        free(cancel);
        pizzeria->selected = i->next;

    }

    return SUCCESS;
}

void free_pizzeria(struct pizzeria *pizzeria) {
    
    if(pizzeria == NULL){
        exit(1);
    }

    struct ingredient *k = pizzeria->stock;
    
    while ( k != NULL ) {

        struct ingredient *temp = k->next;
        free(k);
        k = temp;
    }

    struct order *i = pizzeria->orders;
    while ( i != NULL) {

        struct ingredient *j = i->ingredients;
        while ( j != NULL ) {

            struct ingredient *temp = j->next;
            free(j);
            j = temp;

        }

        struct order *temp = i->next;
        free(i);
        i = temp;
    }

    free(pizzeria);
    pizzeria = NULL;
    return;
}

int refill_stock(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {

    if ( amount <= 0 ) {

        return INVALID_AMOUNT;

    } 

    if ( price < 0 ) {

        return INVALID_PRICE;

    }

    int flag = 0;
    struct ingredient *i = pizzeria->stock;
    while ( i != NULL && flag == 0 ) {

        if ( strcmp(i->name, ingredient_name) == 0 ) {

            i->amount = i->amount + amount;
            flag = 1;

        } else {

            i = i->next;
        }
    }

    if ( i == NULL ) {

        struct ingredient *new = create_ingredient(ingredient_name, amount, price);
        pizzeria->stock = add_ingredient_ordered(new, pizzeria->stock);
    }

    return SUCCESS;
}

void print_stock(struct pizzeria *pizzeria) {

    printf("The stock contains: \n");
    struct ingredient *i = pizzeria->stock;

    while ( i != NULL ) {

        print_ingredient(i->name, i->amount, i->price);
        i = i->next;
    }

    return;
}

int can_complete_order(struct pizzeria *pizzeria) {

    if ( pizzeria->selected == NULL || pizzeria->selected->ingredients == NULL) {

        return INVALID_ORDER;

    }

    int flag = 0;
    struct ingredient *i = pizzeria->selected->ingredients;

    while ( i != NULL) {

        struct ingredient *stock = find_stock(pizzeria->stock, i->name);
        if ( stock == NULL ) {

            flag = 1;
        }
        if ( stock != NULL) {

            if ( i->amount > stock->amount ) {
        
                flag = 1;
            }
        }
        
        i = i->next;
    }
    
    if ( flag == 1 ) {

        return INSUFFICIENT_STOCK;
    }

    return SUCCESS;
}

// r Regular-Base 100 2.0
// r Tomato-Paste 200 1.0
// r Mozzarella-Cheese 4 0.3
// r Basil-Leaf 10 0.15
// s
// o Stanley Margherita 10.99 10
// >
// i Regular-Base 1 2.0
// i Tomato-Paste 1 1.0
// i Mozzarella-Cheese 4 0.3
// i Basil-Leaf 2 0.15
// p
// ?
// q

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

int complete_order(struct pizzeria *pizzeria) {

    if ( pizzeria->selected == NULL ) {

        return INVALID_ORDER;
    }
    if ( pizzeria->selected->ingredients == NULL ) {

        return INVALID_ORDER;
    }

    if ( can_complete_order(pizzeria) == SUCCESS) {

        // printf("ORDER can be completed.\n");

        struct ingredient *i = pizzeria->selected->ingredients;
        struct ingredient *o = NULL;

        while ( i != NULL ) {
            o = find_stock(pizzeria->stock, i->name);

            if ( o != NULL ) {

               o->amount = o->amount - i->amount;
                if ( o->amount <= 0 ) {

                    pizzeria = remove_stock(o, pizzeria);
                }
            }

            i = i->next;
            
        }
        cancel_order(pizzeria);

        return SUCCESS;

    } else {

        return INSUFFICIENT_STOCK;
    }
}

int save_ingredients(struct pizzeria *pizzeria, char *file_name) {

    if(pizzeria->selected == NULL){
        return INVALID_ORDER;
    }

    char buffer[MAX_SAVE_LENGTH];
    struct ingredient *head = pizzeria->selected->ingredients;
    sprintf (buffer, "%s: %d @ $%.2lf\n", head->name, head->amount, head->price);
    
    struct ingredient *i = head->next;
    while ( i != NULL ) {

        char string[MAX_LINE];
        sprintf (string, "%s: %d @ $%.2lf\n", i->name, i->amount, i->price);
        strcat(buffer, string); 
        i = i->next;
    }
   
    save_string(file_name, buffer);
    return SUCCESS;

}
// The C library function "strcat" appends the string pointed to by src to the end of the string pointed to by dest.

int load_ingredients(struct pizzeria *pizzeria, char *file_name) {

    if ( pizzeria->selected == NULL ) {

        return INVALID_ORDER;

    }

    char buffer[MAX_SAVE_LENGTH];
    char* buffer_ptr = buffer;
    buffer_ptr = load_string(file_name);

    char ingredient_name[MAX_STR_LENGTH];
    int amount;
    double price;

    char* ingredient = buffer_ptr;
    char* token = strtok(ingredient, "\n");
    while ( token != NULL ) {

        int length = strlen(token);
        int i = 0;
        while ( i < length - 1) {

            if ( token[i] == ':' || token[i] == '@' || token[i] == '$') {

                token[i] = ' ';
            }
            i++;

        }

        sscanf( token, "%s %d %lf", ingredient_name, &amount,&price); 
        add_ingredient(pizzeria, ingredient_name, amount, price);
        token = strtok(NULL, "\n");
    }

    free(buffer_ptr);
    return SUCCESS;
}
//The C library function strok breaks string str into a series of tokens using the delimiter delim.

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

// Create a new node for a new ingredient
// This function is for Stage 2
struct ingredient *create_ingredient(
    char *ingredient_name, 
    int amount, 
    double price
){
    
    struct ingredient *new = malloc(sizeof( struct ingredient));

    strcpy(new->name, ingredient_name);
    new->amount = amount;
    new->price = price;
    new->next = NULL;

    return new;
}

// Insert ingredient `new` into linked list of ingredients and returns the new head as
// a result
// This function is for Stage 2
struct ingredient *add_ingredient_ordered(
    struct ingredient *new,
    struct ingredient *head
){  

    struct ingredient *previous = NULL;
    struct ingredient *i = head;

    int flag = 0;
    while ( i != NULL && flag == 0 ) {

        if ( strcmp( new->name, i->name) > 0 ) {

           previous = i;
           i = i->next;

        } else {

            flag = 1;
        } 
        
    }
    
    if ( previous == NULL ) {

        head = new;

    } else {

        previous->next = new;

    }
    new->next = i;
    

    return head;
}

// Find the stock in the list and return if found
// This function is for stage 3
struct ingredient *find_stock(
    struct ingredient *head,
    char *stock_name
) {

    int flag = 0;
    struct ingredient *i = head;
    
    while ( i != NULL && flag == 0 ) {

        if ( strcmp(i->name, stock_name) == 0) {

            flag = 1;

        } else {

            i = i->next;

        }
    }


    if ( i == NULL ) {

        return NULL;
    }

    return i;
}

// Removes the required amount from the stock for the order
// This function is for stage 4.
struct pizzeria* remove_stock(
    struct ingredient *stock ,
    struct pizzeria *pizzeria
) {

    struct ingredient *p = pizzeria->stock;

    if ( p == stock) {

        p = p->next;
        free(stock);
        pizzeria->stock = p;

    } else {

        while ( p->next != stock ) {

            p = p->next;
        }

        p->next = stock->next;
        free(stock);
    }

    return pizzeria;
}