#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <algorithm>

struct TypeInfo
{
    std::vector<std::string> advantages;
    std::vector<std::string> disadvantages;
    std::vector<std::string> immunities;
};

std::unordered_map<std::string, TypeInfo> pokemonTypes = {
    {"Normal", TypeInfo{{}, {"Fighting"}, {"Ghost"}}},
    {"Fire", TypeInfo{{"Grass", "Ice", "Bug"}, {"Water", "Rock", "Electric"}, {}}},
    {"Water", TypeInfo{{"Fire", "Ground", "Rock"}, {"Electric", "Grass"}, {}}},
    {"Electric", TypeInfo{{"Water", "Flying"}, {"Ground"}, {}}},
    {"Grass", TypeInfo{{"Water", "Ground", "Rock"}, {"Fire", "Poison", "Flying", "Bug"}, {}}},
    {"Ice", TypeInfo{{"Grass", "Ground", "Flying", "Dragon"}, {"Fire", "Fighting", "Rock"}, {}}},
    {"Fighting", TypeInfo{{"Normal", "Ice", "Rock"}, {"Flying", "Psychic"}, {}}},
    {"Poison", TypeInfo{{"Grass", "Bug"}, {"Ground", "Psychic"}, {}}},
    {"Ground", TypeInfo{{"Fire", "Electric", "Poison", "Rock"}, {"Water", "Grass", "Ice"}, {"Electric"}}},
    {"Flying", TypeInfo{{"Grass", "Fighting", "Bug"}, {"Electric", "Rock"}, {}}},
    {"Psychic", TypeInfo{{"Fighting", "Poison"}, {"Bug"}, {}}},
    {"Bug", TypeInfo{{"Grass", "Psychic"}, {"Fire", "Flying", "Rock"}, {}}},
    {"Rock", TypeInfo{{"Fire", "Ice", "Flying", "Bug"}, {"Water", "Grass", "Fighting", "Ground"}, {}}},
    {"Ghost", TypeInfo{{"Psychic", "Ghost"}, {"Ghost"}, {"Normal", "Fighting"}}},
    {"Dragon", TypeInfo{{"Dragon"}, {"Ice", "Dragon"}, {}}}};

class Move
{
private:
    std::string name, type, stat;
    int power, accuracy, total_power_points, power_points, priority;

public:
    Move() : name("Tackle"), type("Normal"), stat("physical"),
             power(35), accuracy(95), total_power_points(35), power_points(35), priority(0) {}
    Move(std::string name, std::string type, std::string stat, int power, int accuracy, int total_power_points, int priority)
        : name(name), type(type), stat(stat), power(power), accuracy(accuracy),
          total_power_points(total_power_points), power_points(total_power_points), priority(priority) {}

    void reduce_power_points() { power_points--; }

    std::string get_name() { return name; }
    std::string get_type() { return type; }
    std::string get_stat() { return stat; }
    int get_power() { return power; }
    int get_accuracy() { return accuracy; }
    int get_power_points() { return power_points; }
    int get_total_power_points() { return total_power_points; }
    int get_priority() { return priority; }
};

class Pokemon
{
private:
    std::string name, type1, type2;
    int total_hit_points, hit_points, attack, defense, special, speed;
    std::vector<Move> moves;
    bool fainted;

    double calculate_type_modifier(const std::string &move_type, const std::string &target_type1, const std::string &target_type2)
    {
        double modifier = 1.0;
        auto &info_t1 = pokemonTypes.at(target_type1);

        // Verifica imunidades primeiro
        if (std::find(info_t1.immunities.begin(), info_t1.immunities.end(), move_type) != info_t1.immunities.end())
        {
            return 0.0;
        }

        // Aplica STAB (Same-Type Attack Bonus)
        if (move_type == type1 || move_type == type2)
        {
            modifier *= 1.5;
        }

        // Verifica desvantagens (super efetivo)
        if (std::find(info_t1.disadvantages.begin(), info_t1.disadvantages.end(), move_type) != info_t1.disadvantages.end())
        {
            modifier *= 2;
        }
        // Verifica vantagens (não muito efetivo)
        else if (std::find(info_t1.advantages.begin(), info_t1.advantages.end(), move_type) != info_t1.advantages.end())
        {
            modifier *= 0.5;
        }

        // Aplica modificadores para o segundo tipo, se existir
        if (target_type2 != "monotype")
        {
            auto &info_t2 = pokemonTypes.at(target_type2);
            if (std::find(info_t2.immunities.begin(), info_t2.immunities.end(), move_type) != info_t2.immunities.end())
            {
                return 0.0;
            }
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
    Pokemon() : name("Rattata"), type1("Normal"), type2("monotype"),
                total_hit_points(30), attack(56), defense(35), special(25), speed(72),
                hit_points(30), fainted(false), moves({Move(), Move(), Move(), Move()}) {}
    Pokemon(std::string name, std::string type1, std::string type2, int total_hit_points, int attack, int defense,
            int special, int speed, std::vector<Move> moves)
        : name(name), type1(type1), type2(type2), total_hit_points(total_hit_points),
          hit_points(total_hit_points), attack(attack), defense(defense), special(special), speed(speed),
          moves(moves), fainted(false) {}

    bool make_a_move(Pokemon &target_pokemon, int move_idx)
    {
        Move &selected_move = moves[move_idx];
        if (selected_move.get_power_points() > 0)
        {
            std::cout << "\n"
                      << name << " uses " << selected_move.get_name() << "!\n";
            selected_move.reduce_power_points();

            int move_acc = selected_move.get_accuracy();
            int randomNum = rand() % 101;

            if (randomNum <= move_acc)
            {
                const std::string move_stat = selected_move.get_stat();
                const std::string move_type = selected_move.get_type();
                const int move_power = selected_move.get_power();

                const int attack_stat_val = (move_stat == "physical") ? attack : special;
                const int defense_stat_val = (move_stat == "physical") ? target_pokemon.defense : target_pokemon.special;

                double damageTypeModifier = calculate_type_modifier(move_type, target_pokemon.type1, target_pokemon.type2);
                double ratio = static_cast<double>(attack_stat_val) / defense_stat_val;
                int damageBase = static_cast<int>((((2 * 100 / 5) + 2) * move_power * ratio / 50 + 2) * (0.85 + static_cast<double>(rand() % 16) / 100));

                int damage_dealt = damageBase * damageTypeModifier;
                target_pokemon.take_damage(damage_dealt);

                if (damageTypeModifier > 2)
                    std::cout << ">>> SUPER EFFECTIVE! <<<\n";
                else if (damageTypeModifier < 1)
                    std::cout << ">>> Not very effective...\n";
            }
            else
            {
                std::cout << "It missed.\n";
            }
            return true;
        }
        else
        {
            std::cout << "Not enough PP.\n";
            return false;
        }
    }

    void take_damage(int damage)
    {
        if (damage < 0)
            return;
        hit_points -= damage;
        if (hit_points <= 0)
        {
            hit_points = 0;
            fainted = true;
        }
    }

    std::string pokemon_stats_to_string()
    {
        std::string stats = "========================\n";
        stats += name + " (" + type1;
        if (type2 != "monotype")
            stats += "/" + type2;
        stats += ")\nHP:      " + std::to_string(hit_points) + "/" + std::to_string(total_hit_points) + "\n";
        stats += "Attack:  " + std::to_string(attack) + "\nDefense: " + std::to_string(defense) + "\n";
        stats += "Special: " + std::to_string(special) + "\nSpeed:   " + std::to_string(speed) + "\n";
        if (fainted)
            stats += "!!! FAINTED !!!\n";
        stats += "========================\n";
        return stats;
    }

    std::string pokemon_moves_to_string()
    {
        std::string moves_list = "---- Moves ----\n";
        for (int i = 0; i < moves.size(); i++)
        {
            moves_list += "[" + std::to_string(i) + "] " + moves[i].get_name() + " (" + moves[i].get_type() + " | " + moves[i].get_stat() + ")\n";
            moves_list += "POW: " + std::to_string(moves[i].get_power()) +
                          " | ACC: " + std::to_string(moves[i].get_accuracy()) +
                          " | PP: " + std::to_string(moves[i].get_power_points()) + "/" + std::to_string(moves[i].get_total_power_points()) + "\n";
            moves_list += "----------------\n";
        }
        return moves_list;
    }

    std::string get_type1() { return type1; }
    std::string get_type2() { return type2; }
    int get_hit_points() { return hit_points; }
    int get_defense() { return defense; }
    int get_speed() { return speed; }
    int get_special() { return special; }
    Move &get_move(int move_idx) { return moves[move_idx]; }
    bool get_fainted() { return fainted; }
};

class Team
{
private:
    std::vector<Pokemon> pokemon;
    Pokemon &active_pokemon;

public:
    Team() : pokemon({Pokemon(), Pokemon(), Pokemon()}), active_pokemon(pokemon[0]) {}
    Team(std::vector<Pokemon> pokemon, int active_pokemon_idx)
        : pokemon(pokemon), active_pokemon(this->pokemon[active_pokemon_idx]) {}

    void switch_active_pokemon(int active_pokemon_idx)
    {
        if (active_pokemon_idx >= 0 && active_pokemon_idx < pokemon.size() && !pokemon[active_pokemon_idx].get_fainted())
            active_pokemon = pokemon[active_pokemon_idx];
    }

    bool lose()
    {
        int fainted_count = 0;
        for (auto &p : pokemon)
            if (p.get_fainted())
                fainted_count++;
        return fainted_count == pokemon.size();
    }

    std::string to_string()
    {
        std::string team_str = "\n===== Team =====\n";
        for (int i = 0; i < pokemon.size(); i++)
            team_str += "[" + std::to_string(i) + "] " + pokemon[i].pokemon_stats_to_string() + "\n";
        return team_str;
    }

    Pokemon &get_active_pokemon() { return active_pokemon; }
    Pokemon &get_pokemon(int pokemon_idx) { return pokemon[pokemon_idx]; }
    const std::vector<Pokemon> &get_pokemon() const { return pokemon; }
};

class Battle
{
private:
    Team &team1;
    Team &team2;
    Pokemon &team1_active_pokemon;
    Pokemon &team2_active_pokemon;

public:
    Battle(Team &team1, Team &team2)
        : team1(team1), team2(team2),
          team1_active_pokemon(team1.get_active_pokemon()),
          team2_active_pokemon(team2.get_active_pokemon()) {}

    void start()
    {
        while (!team1.lose() && !team2.lose())
        {
            std::cout << "Team 1 active Pokémon:\n"
                      << team1_active_pokemon.pokemon_stats_to_string() << std::endl;
            std::cout << "Team 2 active Pokémon:\n"
                      << team2_active_pokemon.pokemon_stats_to_string() << std::endl;

            int t1_option = -1, t1_suboption = -1;
            int t2_option = -1, t2_suboption = -1;

            do
            {
                std::cout << "\nTeam 1, choose action:\n0 - Attack\n1 - Switch Pokémon\n";
                std::cin >> t1_option;
            } while (t1_option != 0 && t1_option != 1);

            if (t1_option == 0)
            {
                do
                {
                    std::cout << team1_active_pokemon.pokemon_moves_to_string() << "\nChoose move: ";
                    std::cin >> t1_suboption;
                } while (t1_suboption < 0 || t1_suboption >= 4);
            }
            else if (t1_option == 1)
            {
                do
                {
                    std::cout << team1.to_string() << "\nChoose Pokémon to switch: ";
                    std::cin >> t1_suboption;
                } while (t1_suboption < 0 || t1_suboption >= team1.get_pokemon().size() || team1.get_pokemon(t1_suboption).get_fainted());
                team1.switch_active_pokemon(t1_suboption);
                team1_active_pokemon = team1.get_active_pokemon();
            }

            do
            {
                std::cout << "\nTeam 2, choose action:\n0 - Attack\n1 - Switch Pokémon\n";
                std::cin >> t2_option;
            } while (t2_option != 0 && t2_option != 1);

            if (t2_option == 0)
            {
                do
                {
                    std::cout << team2_active_pokemon.pokemon_moves_to_string() << "\nChoose move: ";
                    std::cin >> t2_suboption;
                } while (t2_suboption < 0 || t2_suboption >= 4);
            }
            else if (t2_option == 1)
            {
                do
                {
                    std::cout << team2.to_string() << "\nChoose Pokémon to switch: ";
                    std::cin >> t2_suboption;
                } while (t2_suboption < 0 || t2_suboption >= team2.get_pokemon().size() || team2.get_pokemon(t2_suboption).get_fainted());
                team2.switch_active_pokemon(t2_suboption);
                team2_active_pokemon = team2.get_active_pokemon();
            }

            if (t1_option == 0 && t2_option == 0)
            {
                Move &move1 = team1_active_pokemon.get_move(t1_suboption);
                Move &move2 = team2_active_pokemon.get_move(t2_suboption);

                int priority1 = move1.get_priority();
                int priority2 = move2.get_priority();

                if (priority1 > priority2 || (priority1 == priority2 && (team1_active_pokemon.get_speed() > team2_active_pokemon.get_speed() || (team1_active_pokemon.get_speed() == team2_active_pokemon.get_speed() && rand() % 2 == 0))))
                {
                    team1_active_pokemon.make_a_move(team2_active_pokemon, t1_suboption);
                    if (team2_active_pokemon.get_fainted())
                    {
                        std::cout << "Team 2 Pokémon fainted!\n";

                        if (team2.lose())
                        {
                            break;
                        }

                        std::cout << "Choose a new Pokémon:\n";
                        int new_idx;
                        do
                        {
                            std::cout << team2.to_string() << "\nChoose: ";
                            std::cin >> new_idx;
                        } while (new_idx < 0 || new_idx >= team2.get_pokemon().size() || team2.get_pokemon(new_idx).get_fainted());
                        team2.switch_active_pokemon(new_idx);
                        team2_active_pokemon = team2.get_active_pokemon();
                    }
                    else
                    {
                        team2_active_pokemon.make_a_move(team1_active_pokemon, t2_suboption);
                    }
                }
                else
                {
                    team2_active_pokemon.make_a_move(team1_active_pokemon, t2_suboption);
                    if (team1_active_pokemon.get_fainted())
                    {
                        std::cout << "Team 1 Pokémon fainted!\n";
                        if (team1.lose())
                        {
                            break;
                        }
                        std::cout << "Choose a new Pokémon:\n";

                        int new_idx;
                        do
                        {
                            std::cout << team1.to_string() << "\nChoose: ";
                            std::cin >> new_idx;
                        } while (new_idx < 0 || new_idx >= team1.get_pokemon().size() || team1.get_pokemon(new_idx).get_fainted());
                        team1.switch_active_pokemon(new_idx);
                        team1_active_pokemon = team1.get_active_pokemon();
                    }
                    else
                    {
                        team1_active_pokemon.make_a_move(team2_active_pokemon, t1_suboption);
                    }
                }
            }
            else
            {
                if (t1_option == 0)
                {
                    team1_active_pokemon.make_a_move(team2_active_pokemon, t1_suboption);
                    if (team2_active_pokemon.get_fainted())
                    {
                        std::cout << "Team 2 Pokémon fainted!\n";
                        if (team2.lose())
                        {
                            break;
                        }
                        std::cout << "Choose a new Pokémon:\n";
                        int new_idx;
                        do
                        {
                            std::cout << team2.to_string() << "\nChoose: ";
                            std::cin >> new_idx;
                        } while (new_idx < 0 || new_idx >= team2.get_pokemon().size() || team2.get_pokemon(new_idx).get_fainted());
                        team2.switch_active_pokemon(new_idx);
                        team2_active_pokemon = team2.get_active_pokemon();
                    }
                }
                if (t2_option == 0)
                {
                    team2_active_pokemon.make_a_move(team1_active_pokemon, t2_suboption);
                    if (team1_active_pokemon.get_fainted())
                    {
                        std::cout << "Team 1 Pokémon fainted!\n";
                        if (team1.lose())
                        {
                            break;
                        }
                        std::cout << "Choose a new Pokémon:\n";
                        int new_idx;
                        do
                        {
                            std::cout << team1.to_string() << "\nChoose: ";
                            std::cin >> new_idx;
                        } while (new_idx < 0 || new_idx >= team1.get_pokemon().size() || team1.get_pokemon(new_idx).get_fainted());
                        team1.switch_active_pokemon(new_idx);
                        team1_active_pokemon = team1.get_active_pokemon();
                    }
                }
            }

            std::cout << "\n--- End of Turn ---\n";
        }

        if (team1.lose())
            std::cout << "Team 2 wins!\n";
        else if (team2.lose())
            std::cout << "Team 1 wins!\n";
    }
};

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    Move tackle("Tackle", "Normal", "physical", 35, 95, 35, 0);
    Move ember("Ember", "Fire", "special", 40, 100, 25, 0);
    Move water_gun("Water Gun", "Water", "special", 40, 100, 25, 0);
    Move thunder_shock("Thunder Shock", "Electric", "special", 40, 100, 25, 0);
    Move vine_whip("Vine Whip", "Grass", "physical", 45, 100, 25, 0);
    Move ice_beam("Ice Beam", "Ice", "special", 90, 100, 10, 0);
    Move psychic("Psychic", "Psychic", "special", 90, 100, 10, 0);
    Move earthquake("Earthquake", "Ground", "physical", 100, 100, 10, 0);
    Move fly("Fly", "Flying", "physical", 90, 95, 15, 0);

    Pokemon p2("Charmander", "Fire", "monotype", 39, 52, 43, 60, 65, {ember, tackle, tackle, tackle});
    Pokemon p3("Squirtle", "Water", "monotype", 44, 48, 65, 50, 43, {water_gun, tackle, tackle, tackle});

    Team team1({p2}, 0);
    Team team2({p3}, 0);

    Battle battle(team1, team2);
    battle.start();

    return 0;
}