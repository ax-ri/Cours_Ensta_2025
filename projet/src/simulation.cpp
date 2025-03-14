#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <thread>

#include "display.hpp"
#include "model.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

struct ParamsType {
  ParamsType() : display(true), dump(false), check(false) {}
  double length{1.};
  unsigned discretization{20u};
  std::array<double, 2> wind{0., 0.};
  Model::LexicoIndices start{10u, 10u};
  bool display;
  bool dump;
  bool check;
};

void analyze_arg(int nargs, char *args[], ParamsType &params) {
  if (nargs == 0)
    return;
  std::string key(args[0]);
  if (key == "-l"s) {
    if (nargs < 2) {
      std::cerr << "Manque une valeur pour la longueur du terrain !"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    params.length = std::stoul(args[1]);
    analyze_arg(nargs - 2, &args[2], params);
    return;
  }
  auto pos = key.find("--longueur=");
  if (pos < key.size()) {
    auto subkey = std::string(key, pos + 11);
    params.length = std::stoul(subkey);
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }

  if (key == "-n"s) {
    if (nargs < 2) {
      std::cerr << "Manque une valeur pour le nombre de cases par direction "
                   "pour la discrétisation du terrain !"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    params.discretization = std::stoul(args[1]);
    analyze_arg(nargs - 2, &args[2], params);
    return;
  }
  pos = key.find("--number_of_cases=");
  if (pos < key.size()) {
    auto subkey = std::string(key, pos + 18);
    params.discretization = std::stoul(subkey);
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }

  if (key == "-w"s) {
    if (nargs < 2) {
      std::cerr << "Manque une paire de valeurs pour la direction du vent !"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string values = std::string(args[1]);
    params.wind[0] = std::stod(values);
    auto pos = values.find(",");
    if (pos == values.size()) {
      std::cerr << "Doit fournir deux valeurs séparées par une virgule pour "
                   "définir la vitesse"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    auto second_value = std::string(values, pos + 1);
    params.wind[1] = std::stod(second_value);
    analyze_arg(nargs - 2, &args[2], params);
    return;
  }
  pos = key.find("--wind=");
  if (pos < key.size()) {
    auto subkey = std::string(key, pos + 7);
    params.wind[0] = std::stoul(subkey);
    auto pos = subkey.find(",");
    if (pos == subkey.size()) {
      std::cerr << "Doit fournir deux valeurs séparées par une virgule pour "
                   "définir la vitesse"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    auto second_value = std::string(subkey, pos + 1);
    params.wind[1] = std::stod(second_value);
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }

  if (key == "-s"s) {
    if (nargs < 2) {
      std::cerr
          << "Manque une paire de valeurs pour la position du foyer initial !"
          << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string values = std::string(args[1]);
    params.start.column = std::stod(values);
    auto pos = values.find(",");
    if (pos == values.size()) {
      std::cerr << "Doit fournir deux valeurs séparées par une virgule pour "
                   "définir la position du foyer initial"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    auto second_value = std::string(values, pos + 1);
    params.start.row = std::stod(second_value);
    analyze_arg(nargs - 2, &args[2], params);
    return;
  }
  pos = key.find("--start=");
  if (pos < key.size()) {
    auto subkey = std::string(key, pos + 8);
    params.start.column = std::stoul(subkey);
    auto pos = subkey.find(",");
    if (pos == subkey.size()) {
      std::cerr << "Doit fournir deux valeurs séparées par une virgule pour "
                   "définir la vitesse"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    auto second_value = std::string(subkey, pos + 1);
    params.start.row = std::stod(second_value);
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }

  pos = key.find("--no-display");
  if (pos < key.size()) {
    params.display = false;
    analyze_arg(nargs - 1, &args[1], params);
  }

  if (key == "-d"s) {
    params.dump = true;
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }
  pos = key.find("--dump");
  if (pos < key.size()) {
    params.dump = true;
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }

  if (key == "-c"s) {
    params.check = true;
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }
  pos = key.find("--check");
  if (pos < key.size()) {
    params.check = true;
    analyze_arg(nargs - 1, &args[1], params);
    return;
  }
}

ParamsType parse_arguments(int nargs, char *args[]) {
  if (nargs == 0)
    return {};
  if ((std::string(args[0]) == "--help"s) || (std::string(args[0]) == "-h")) {
    std::cout <<
        R"RAW(Usage : simulation [option(s)]
  Lance la simulation d'incendie en prenant en compte les [option(s)].
  Les options sont :
    -l, --longueur=LONGUEUR     Définit la taille LONGUEUR (réel en km) du carré représentant la carte de la végétation.
    -n, --number_of_cases=N     Nombre n de cases par direction pour la discrétisation
    -w, --wind=VX,VY            Définit le vecteur vitesse du vent (pas de vent par défaut).
    -s, --start=COL,ROW         Définit les indices I,J de la case où commence l'incendie (milieu de la carte par défaut)
    --no-display                Désactive l'affichage de la simulation
    -d, --dump                  Dump le contenu de la simulation toutes les 32 itérations
    -c, --check                 Vérifie que la simulation est la même que le dump (nécessite --dump au préalable)
)RAW";
    exit(EXIT_SUCCESS);
  }
  ParamsType params;
  analyze_arg(nargs, args, params);
  return params;
}

bool check_params(ParamsType &params) {
  bool flag = true;
  if (params.length <= 0) {
    std::cerr << "[ERREUR FATALE] La longueur du terrain doit être positive et "
                 "non nulle !"
              << std::endl;
    flag = false;
  }

  if (params.discretization <= 0) {
    std::cerr << "[ERREUR FATALE] Le nombre de cellules par direction doit "
                 "être positive et non nulle !"
              << std::endl;
    flag = false;
  }

  if ((params.start.row >= params.discretization) ||
      (params.start.column >= params.discretization)) {
    std::cerr
        << "[ERREUR FATALE] Mauvais indices pour la position initiale du foyer"
        << std::endl;
    flag = false;
  }

  return flag;
}

void display_params(ParamsType const &params) {
  std::cout << "Parametres définis pour la simulation : \n"
            << "\tTaille du terrain : " << params.length << std::endl
            << "\tNombre de cellules par direction : " << params.discretization
            << std::endl
            << "\tVecteur vitesse : [" << params.wind[0] << ", "
            << params.wind[1] << "]" << std::endl
            << "\tPosition initiale du foyer (col, ligne) : "
            << params.start.column << ", " << params.start.row << std::endl
            << "\tAffichage: " << (params.display ? "yes" : "no") << std::endl
            << "\tDumping: " << (params.dump ? "yes" : "no") << std::endl;
}

void dumpSimulation(const Model &simu) {
  std::ofstream outFile("dump/" + std::to_string(simu.time_step()) + ".txt");
  const auto &fireMap = simu.fire_map();
  const auto &vegetalMap = simu.vegetal_map();
  for (unsigned int i = 0; i < fireMap.size(); ++i) {
    outFile << (int)fireMap[i] << " ";
  }
  outFile << "\n";
  for (unsigned int i = 0; i < vegetalMap.size(); ++i) {
    outFile << (int)vegetalMap[i] << " ";
  }
  outFile.close();
}

void checkSimulation(const Model &simu) {
  std::ifstream inFile("dump/" + std::to_string(simu.time_step()) + ".txt");
  const auto &fireMap = simu.fire_map();
  const auto &vegetalMap = simu.vegetal_map();
  int x;
  for (unsigned int i = 0; i < fireMap.size(); ++i) {
    inFile >> x;
    if ((int)fireMap[i] != x) {
      std::cout << "expected " << (int)fireMap[i] << " got " << x << std::endl;
    }
  }
  // for (unsigned int i = 0; i < vegetalMap.size(); ++i) {
  //   inFile >> x;
  //   assert((int)vegetalMap[i] == x);
  // }
  inFile.close();
}

int main(int nargs, char *args[]) {
  auto params = parse_arguments(nargs - 1, &args[1]);
  display_params(params);
  if (!check_params(params))
    return EXIT_FAILURE;

  auto displayer = params.display
                       ? Displayer::init_instance(params.discretization,
                                                  params.discretization)
                       : nullptr;
#pragma omp parallel
  {
    if (omp_get_thread_num() == 0) {
      std::cout << "Running with " << omp_get_num_threads() << " threads"
                << std::endl;
    }
  }

  if (params.dump) {
    // clear output dir
    std::filesystem::remove_all("dump");
    std::filesystem::create_directory("dump");
  }

  auto simu =
      Model(params.length, params.discretization, params.wind, params.start);
  SDL_Event event;
  bool stop = false;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  auto displayDuration = std::chrono::duration<double>::zero();
  start = std::chrono::system_clock::now();
  int itCount = 0;

  while (simu.update() && !stop) {
    if ((simu.time_step() & 31) == 0) {
      std::cout << "Time step " << simu.time_step()
                << "\n===============" << std::endl;
      if (params.dump) {
        dumpSimulation(simu);
      } else if (params.check) {
        checkSimulation(simu);
      }
    }
    if (params.display) {
      std::chrono::time_point<std::chrono::system_clock> displayStart,
          displayEnd;
      displayStart = std::chrono::system_clock::now();
      displayer->update(simu.vegetal_map(), simu.fire_map());
      displayEnd = std::chrono::system_clock::now();
      displayDuration += displayEnd - displayStart;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          stop = true;
          break;
        }
      }
    }

    ++itCount;
    // std::this_thread::sleep_for(0.1s);
  }

  end = std::chrono::system_clock::now();

  const std::chrono::duration<double> duration = end - start;
  std::cout << "Average whole time step duration: "
            << (float)duration.count() / (float)itCount << " seconds"
            << std::endl;
  std::cout << "Average display step only duration: "
            << (float)displayDuration.count() / (float)itCount << " seconds"
            << std::endl;

  return EXIT_SUCCESS;
}
