#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <mpi.h>

#include "display.hpp"
#include "model.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

struct ParamsType {
  double length{1.};
  unsigned discretization{20u};
  std::array<double, 2> wind{0., 0.};
  Model::LexicoIndices start{10u, 10u};
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
            << params.start.column << ", " << params.start.row << std::endl;
}

int main(int nargs, char *args[]) {
  // Initialisation de MPI
  MPI_Init(&nargs, &args);
  int rank, size;
  MPI_Comm globComm;
  MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
  MPI_Comm_rank(globComm, &rank);
  MPI_Comm_size(globComm, &size);
  std::cout << "Hello from " << rank << " of " << size << std::endl;

    // verif nombre processeurs

  auto params = parse_arguments(nargs - 1, &args[1]);
  if (!check_params(params))
    return EXIT_FAILURE;

  // On affiche sur le proc 0
  // Calcul sur le proc 1
  bool stop = false;
  

  if (rank == 0){
    display_params(params);
    auto displayer = Displayer::init_instance(params.discretization, params.discretization);
    SDL_Event event;
    // définition des tableaux dans model.hpp :
    // std::vector<std::uint8_t> m_vegetation_map, m_fire_map;
    std::size_t map_size = params.discretization * params.discretization;
    std::vector<std::uint8_t> vegetal_map_data(map_size);
    std::vector<std::uint8_t> fire_map_data(map_size);



    while(!stop){
      // recevoir les données
      MPI_Recv(vegetal_map_data.data(), map_size, MPI_UINT8_T, 1, 0, globComm, MPI_STATUS_IGNORE);
      MPI_Recv(fire_map_data.data(), map_size, MPI_UINT8_T, 1, 1, globComm, MPI_STATUS_IGNORE);
      displayer->update(vegetal_map_data, fire_map_data);

      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          stop = true;
          break;
        }
      }
      MPI_Send(&stop, 1, MPI_C_BOOL, 1, 2, globComm);
    }
  }

  else if (rank == 1){
    Model simu(params.length, params.discretization, params.wind, params.start);
    std::size_t map_size = params.discretization * params.discretization;
    std::vector<std::uint8_t> vegetal_map_data(map_size);
    std::vector<std::uint8_t> fire_map_data(map_size);

    while(simu.update() && !stop){
        if ((simu.time_step() & 31) == 0)
          std::cout << "Time step " << simu.time_step()
                    << "\n===============" << std::endl;
      // envoyer les données
      vegetal_map_data = simu.vegetal_map();
      fire_map_data = simu.fire_map();
      MPI_Send(vegetal_map_data.data(), map_size, MPI_UINT8_T, 0, 0, globComm);
      MPI_Send(fire_map_data.data(), map_size, MPI_UINT8_T, 0, 1, globComm);
      MPI_Recv(&stop, 1, MPI_C_BOOL, 0, 2, globComm, MPI_STATUS_IGNORE);
    }
  }

  // auto displayer = rank == 0 ? Displayer::init_instance(params.discretization, params.discretization) : nullptr;
  // auto simu = rank == 0? nullptr : Model(params.length, params.discretization, params.wind, params.start);
  // SDL_Event event;MPI_Comm globComm;
  // bool stop = false;
  // while (simu.update() && !stop) {
  //   if ((simu.time_step() & 31) == 0)
  //     std::cout << "Time step " << simu.time_step()
  //               << "\n===============" << std::endl;
  //   displayer->update(simu.vegetal_map(), simu.fire_map());
  //   while (SDL_PollEvent(&event)) {
  //     if (event.type == SDL_QUIT) {
  //       stop = true;
  //       break;
  //     }
  //   }
  //   std::this_thread::sleep_for(0.1s);
  // }
  MPI_Finalize();
  return EXIT_SUCCESS;
}
