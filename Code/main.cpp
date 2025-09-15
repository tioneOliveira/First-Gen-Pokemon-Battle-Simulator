#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>

struct TypeInfo
{
    std::vector<std::string> advantages;
    std::vector<std::string> disadvantages;
};

std::unordered_map<std::string, TypeInfo> pokemonTypes = {
    {"Normal", TypeInfo{{}, {"Fighting"}}},
    {"Fire", TypeInfo{{"Grass", "Ice", "Bug"}, {"Water", "Rock", "Electric"}}},
    {"Water", TypeInfo{{"Fire", "Ground", "Rock"}, {"Electric", "Grass"}}},
    {"Electric", TypeInfo{{"Water", "Flying"}, {"Ground"}}},
    {"Grass", TypeInfo{{"Water", "Ground", "Rock"}, {"Fire", "Poison", "Flying", "Bug"}}},
    {"Ice", TypeInfo{{"Grass", "Ground", "Flying", "Dragon"}, {"Fire", "Fighting", "Rock"}}},
    {"Fighting", TypeInfo{{"Normal", "Ice", "Rock"}, {"Flying", "Psychic"}}},
    {"Poison", TypeInfo{{"Grass", "Bug"}, {"Ground", "Psychic"}}},
    {"Ground", TypeInfo{{"Fire", "Electric", "Poison", "Rock"}, {"Water", "Grass", "Ice"}}},
    {"Flying", TypeInfo{{"Grass", "Fighting", "Bug"}, {"Electric", "Rock"}}},
    {"Psychic", TypeInfo{{"Fighting", "Poison"}, {"Bug"}}},
    {"Bug", TypeInfo{{"Grass", "Psychic"}, {"Fire", "Flying", "Rock"}}},
    {"Rock", TypeInfo{{"Fire", "Ice", "Flying", "Bug"}, {"Water", "Grass", "Fighting", "Ground"}}},
    {"Ghost", TypeInfo{{"Psychic", "Ghost"}, {"Ghost"}}},
    {"Dragon", TypeInfo{{"Dragon"}, {"Ice", "Dragon"}}}};

class Move
{
private:
    std::string name, type, stat;
    int power, accuracy, power_points;

public:
    Move(std::string name, std::string type, std::string stat, int power, int accuracy, int power_points)
    {
        this->name = name;
        this->type = type;
        this->stat = stat;
        this->power = power;
        this->accuracy = accuracy;
        this->power_points = power_points;
    }

    void reduce_power_points()
    {
        power_points--;
    }

    // Getters
    std::string get_name()
    {
        return name;
    }
    std::string get_type()
    {
        return type;
    }
    std::string get_stat()
    {
        return stat;
    }

    int get_power()
    {
        return power;
    }
    int get_accuracy()
    {
        return accuracy;
    }
    int get_power_points()
    {
        return power_points;
    }
};

class Pokemon
{
private:
    std::string name, type1, type2;
    int total_hit_points, hit_points, attack, defense, special, speed;
    std::vector<Move> moves;

    double calculate_type_modifier(const std::string &move_type, const std::string &target_type1, const std::string &target_type2)
    {
        double modifier = 1.0;
        auto &info_t1 = pokemonTypes[target_type1];

        if (move_type == type1 || move_type == type2)
        {
            modifier *= 1.5;
        }

        if (std::find(info_t1.disadvantages.begin(), info_t1.disadvantages.end(), move_type) != info_t1.disadvantages.end())
        {
            modifier *= 2;
        }
        else if (std::find(info_t1.advantages.begin(), info_t1.advantages.end(), move_type) != info_t1.advantages.end())
        {
            modifier *= 0.5;
        }

        if (target_type2 != "monotype")
        {
            auto &info_t2 = pokemonTypes[target_type2];
            if (std::find(info_t2.disadvantages.begin(), info_t2.disadvantages.end(), move_type) != info_t2.disadvantages.end())
            {
                modifier *= 2;
            }
            else if (std::find(info_t2.advantages.begin(), info_t2.advantages.end(), move_type) != info_t2.advantages.end())
            {
                modifier *= 0.5;
            }
        }

        return modifier;
    }

public:
    Pokemon(std::string name, std::string type1, std::string type2, int total_hit_points, int attack, int defense, int special, int speed, std::vector<Move> moves)
    {
        this->name = name;

        this->type1 = type1;
        this->type2 = type2;

        this->total_hit_points = total_hit_points;

        hit_points = total_hit_points;
        this->attack = attack;
        this->defense = defense;
        this->special = special;
        this->speed = speed;

        this->moves = moves;
    }

    void make_a_move(Pokemon &target_pokemon, int move_idx)
    {
        Move selected_move = moves[move_idx];

        int selected_move_power_points = selected_move.get_power_points();

        if (selected_move_power_points > 0)
        {

            std::string move_name = selected_move.get_name();

            std::cout << name + " use " + move_name << std::endl;

            selected_move.reduce_power_points();

            int move_acc = selected_move.get_accuracy();
            int randomNum = rand() % 101;

            if (randomNum <= move_acc)
            {

                const std::string move_stat = selected_move.get_stat();
                const std::string move_type = selected_move.get_type();
                const int move_power = selected_move.get_power();

                const std::string target_t1 = target_pokemon.get_type1();
                const std::string target_t2 = target_pokemon.get_type2();
                const int target_defense = target_pokemon.get_defense();
                const int target_special = target_pokemon.get_special();
                const int target_hit_points = target_pokemon.get_hit_points();

                const int attack_stat_val = move_stat == "physical" ? attack : special;
                const int defense_stat_val = move_stat == "physical" ? target_defense : target_special;

                const double damageTypeModfier = calculate_type_modifier(move_type, target_t1, target_t2);
                const int damageBase = ((((2 * 100 * 1) / 5) + 2) * ((move_power * (attack_stat_val / defense_stat_val)) / 50) + 2);

                const int damage_dealt = damageBase * damageTypeModfier;

                target_pokemon.take_damage(damage_dealt);

                if (damageTypeModfier > 2)
                    std::cout << "It's super effective!" << std::endl;
                else if (damageTypeModfier < 1)
                    std::cout << "It's not very effective." << std::endl;
            }

            std::cout << "It missed." << std::endl;
        }
        else
        {
            std::cout << "Not enought PP." << std::endl;
        }
    }
    void take_damage(int damage)
    {
        hit_points -= damage;
    }

    // Getters
    std::string get_type1()
    {
        return type1;
    }
    std::string get_type2()
    {
        return type2;
    }
    int get_hit_points()
    {
        return hit_points;
    }
    int get_defense()
    {
        return defense;
    }
    int get_special()
    {
        return special;
    }
};
