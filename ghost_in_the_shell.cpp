
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <optional>
#include <cmath>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 enum eOwner{
    eOwner_opponent = -1,
    eOwner_neutral,
    eOwner_me
};

 typedef struct Troop {
    int id;
    int owner;
    int leftFromFactory;
    int targetFactory;
    int numOfCyborgs;
    int turnsTillArrive;
} troop;

typedef struct Factory {
    int id;
    int owner;
    int cyborgs;
    int production;
    int unused;
    int unused2;
 } factory;

typedef struct Links {
    int factory1;
    int factory2;
    int distance;
} links;


bool factory_belongs_to(vector<factory> factory_list, int id)
{
    for (auto fact : factory_list) {
        if ( fact.id == id )
            return true;
    }
    return false;
    //return ( find(factory_list.begin(), factory_list.end(), [id](Factory fac){ return fac.id == id;}) != factory_list.begin());
}

std::optional<factory> get_facoroty_by_id(vector<factory> factory_list, int id)
{
    for (auto fact : factory_list) {
        if ( fact.id == id )
            return fact;
    }
    return {};
}

links get_link_from_src_dest(int src, int dest, vector<links> all_links)
{
    links ret_val;
    for (auto lnk : all_links){
        if ((lnk.factory1 == src) && (lnk.factory2 == dest))
        {
            ret_val = lnk;
        }
        if ((lnk.factory1 == dest) && (lnk.factory2 == src))
        {
            ret_val = lnk;
        }

    }
    return ret_val;
}

//TOTO: Multi moves arent handled correctly
//TODO: Increase manufacurer not handled correctly
//any base with more than 20 cyborgs has to move them or consume them, so not bomb target

int main()
{
    int bombs = 2;
    int bomb_counter = 0;
    vector<links> factory_links;
    int factoryCount; // the number of factories
    cin >> factoryCount; cin.ignore();
    int linkCount; // the number of links between factories
    cin >> linkCount; cin.ignore();
    for (int i = 0; i < linkCount; i++) {
        int factory1;
        int factory2;
        int distance;
        cin >> factory1 >> factory2 >> distance; cin.ignore();
        factory_links.push_back({factory1,factory2,distance});
    }

    // game loop
    while (1) {
        vector<factory> my_factories;
        vector<factory> nm_factories;
        vector<factory> nt_factories;
        vector<troop> vec_troop;
        cerr << "bombs left: " << bombs << " cntr" << bomb_counter << endl;

        int entityCount; // the number of entities (e.g. factories and troops)
        cin >> entityCount; cin.ignore();
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            string entityType;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            int arg5;
            cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();
            if ( entityType == "FACTORY" )
                if ( arg1 == eOwner_me ) {
                    my_factories.push_back({entityId,arg1,arg2,arg3,arg4,arg5});
                }
                else if ( arg1 == eOwner_neutral) {
                    nm_factories.push_back({entityId,arg1,arg2,arg3,arg4,arg5});
                }
                else if ( arg1 == eOwner_opponent) {
                    nm_factories.push_back({entityId,arg1,arg2,arg3,arg4,arg5});
                    nt_factories.push_back({entityId,arg1,arg2,arg3,arg4,arg5});
                }
                else {
                    cerr << "Invalid state of owner" << endl;
                }
            else if ( entityType == "TROOP" ) 
                vec_troop.push_back({entityId,arg1,arg2,arg3,arg4,arg5});
            else
                cerr << "Weird state for entiry, did i make a mistake" << endl;
        }

        int most_cyborgs = 0;
        factory source;
        string upgrade = "";

        for (auto my_fact : my_factories) {      
            if ( my_fact.cyborgs > most_cyborgs )
            {
                most_cyborgs = my_fact.cyborgs;
                source = my_fact;
                cerr << my_fact.cyborgs <<endl;

            }
            if ((( my_fact.production < 1) && (my_fact.cyborgs > 10)) || ((my_factories.size() == 1) && ( my_fact.production < 2 )) )
            {
                upgrade = ";INC " + to_string(my_fact.id);
                my_fact.cyborgs -= 10;
            }
            if ((my_fact.cyborgs > 20 ) && ( my_fact.production < 3 ))
            {
                upgrade += ";INC " + to_string(my_fact.id);
                my_fact.cyborgs -= 10;
            }
        }


        vector<int> destination_list;
        for(auto f_link : factory_links) {
            if (( f_link.factory1 == source.id ) && ( !factory_belongs_to(my_factories, f_link.factory2) )) {
                destination_list.push_back(f_link.factory2);
            }
            if ((f_link.factory2 == source.id ) && ( !factory_belongs_to(my_factories, f_link.factory1) )) {
                destination_list.push_back(f_link.factory1);
            }
        }

        int best_production = 0;
        int nme_best_production = 0;
        //Only ever target two
        factory destination = {-1,-1,-1,-1};
        factory destination2 = {-1,-1,-1,-1};
        string bomb = "";
        int short_opt;
        for (auto dest : destination_list)
        {
            factory potential = get_facoroty_by_id(nm_factories,dest).value_or(source);

            cerr << "id:" << potential.id <<" " << potential.owner <<" "  <<potential.production <<" " <<potential.cyborgs << " " << bombs <<" " << bomb_counter << endl;
            if ((potential.owner == eOwner_opponent) && (potential.production > 1) && (potential.cyborgs > 16) 
                && (bombs > 0) && (bomb_counter < 1) )
            {
                bomb = ";BOMB " + to_string(source.id) + " " + to_string(potential.id);
                bomb_counter = get_link_from_src_dest(source.id, potential.id, factory_links).distance+5;
            }

            if (source.cyborgs < potential.cyborgs)
                continue;

            if ((potential.production ==  best_production) && (potential.owner == eOwner_neutral )){
                best_production = potential.production;
                destination = potential;
            }
            if (potential.production ==  best_production) {
                if (get_link_from_src_dest(source.id, potential.id, factory_links).distance < get_link_from_src_dest(source.id, destination.id, factory_links).distance )
                {
                    best_production = potential.production;
                    destination = potential;
                }
            }
            if (potential.production > best_production) {
                best_production = potential.production;
                destination = potential;
            }

            if ( (potential.owner == eOwner_neutral) && (potential.production == 0))
            {
                destination2 = potential;
            }

            //BEST DOESN'T HANDLE IF TARGET IS CONNECTED TO LAST ENEMY LOCATION
        }


        // Any valid action, such as "WAIT" or "MOVE source destination cyborgs"
        if ((source.cyborgs < 5) || (destination.production == -1))
            cout << "WAIT";
            //See if we can upgrade any factories rather than waste a turn waiting. 
        else if (source.cyborgs > 20) {
            cout << "MOVE " << source.id << " " << destination.id << " " << floor(source.cyborgs *.08);
        }
        else if (destination2.id != -1) {
            cout << "MOVE " << source.id 
                 << " " << destination.id 
                 << " " << 3
                 << ";MOVE " << source.id 
                 << " " << destination2.id 
                 << " " << 1;
        }
        
        else {
            cout << "MOVE " << source.id << " " << destination.id << " " << 3;
        }
        
        cout << bomb << upgrade << endl;
        bomb_counter--;
    }
}