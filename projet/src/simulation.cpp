#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <mpi.h>
#include <time.h>

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

void update_ghost_cells(std::vector<std::uint8_t> &map, int n, int rank, int size){
  // envoie les ghosts cells
  // std::cout << "rank " << rank << " size " << size << std::endl;
  if (rank != 1){
    MPI_Send(map.data(), n, MPI_UINT8_T, rank - 1, 0, MPI_COMM_WORLD);
    MPI_Recv(map.data(), n, MPI_UINT8_T, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  if (rank != size - 1){
    MPI_Send(map.data() + (n - 1) * n, n, MPI_UINT8_T, rank + 1, 1, MPI_COMM_WORLD);
    MPI_Recv(map.data() + n * n, n, MPI_UINT8_T, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}

int main(int nargs, char *args[]) {
  // Initialisation de MPI
  MPI_Init(&nargs, &args);
  int rank, size;
  MPI_Comm globComm;
  MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
  MPI_Comm_rank(globComm, &rank);
  MPI_Comm_size(globComm, &size);

  // verif nombre processeurs
  if (size < 2){
    if (rank == 0)
      std::cerr << "Il faut au moins 2 processeurs" << std::endl;
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  // Création d'un groupe de communication pour les calculs
  // Tous les processus sauf le 0 y sont
  int color = (rank == 0) ? MPI_UNDEFINED : 0;
  MPI_Comm calcComm;
  MPI_Comm_split(globComm, color, rank, &calcComm);

  auto params = parse_arguments(nargs - 1, &args[1]);
  if (!check_params(params))
    return EXIT_FAILURE;

  // On affiche sur le proc 0
  // Calcul sur le proc 1
  bool stop = false;
  int itCount = 0;

  // Taille des tableaux
  int n = params.discretization;


  if (rank == 0){
    display_params(params);
    auto displayer = Displayer::init_instance(params.discretization, params.discretization);
    SDL_Event event;
    // définition des tableaux dans model.hpp :
    // std::vector<std::uint8_t> m_vegetation_map, m_fire_map;
    std::size_t map_size = n * n;
    std::vector<std::uint8_t> vegetal_map_data(map_size);
    std::vector<std::uint8_t> fire_map_data(map_size);

    std::chrono::time_point<std::chrono::system_clock> displayStart = std::chrono::system_clock::now();

    while(!stop){
      itCount++;
      // recevoir les données
      MPI_Recv(vegetal_map_data.data(), map_size, MPI_UINT8_T, 1, 0, globComm, MPI_STATUS_IGNORE);
      MPI_Recv(fire_map_data.data(), map_size, MPI_UINT8_T, 1, 1, globComm, MPI_STATUS_IGNORE);
      // affichage
      displayer->update(vegetal_map_data, fire_map_data);

      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          stop = true;
          break;
        }
      }
      MPI_Send(&stop, 1, MPI_C_BOOL, 1, 2, globComm);
      MPI_Recv(&stop, 1, MPI_C_BOOL, 1, 3, globComm, MPI_STATUS_IGNORE);
    }
    std::chrono::time_point<std::chrono::system_clock> displayEnd = std::chrono::system_clock::now();
    auto displayDuration = displayEnd - displayStart;
    std::cout << "Average display step only duration: "
            << (float)displayDuration.count() / (float)itCount << " seconds"
            << std::endl;
  }

  // si pas proc 1, calculs
  else if (rank != 0){
    // Découpage de la simulation en tranches
    int rows_per_proc = n / (size - 1);
    int start_row = (rank - 1) * rows_per_proc;
    // ne pas oublier que la dernière tranche n'est pas forcément "pleine"
    int end_row = (rank == size - 1) ? n : start_row + rows_per_proc;
    // gère les ghosts cells
    if (rank != 1)
      start_row--;
    if (rank != size - 1)
      end_row++;
    // taille tranches
    int local_n = end_row - start_row;
    // length de la carte locale
    double local_length = params.length / n * local_n;

    // initialisation de la simulation
    Model simu_loc(local_length, local_n, params.wind, params.start);
    std::size_t map_size = n * local_n;

    // données locales, pour l'envoi
    std::vector<std::uint8_t> vegetal_map_data(map_size);
    std::vector<std::uint8_t> fire_map_data(map_size);

    // pour regroupement
    // ce serait mieux de ne le créer que pour le proc 1
    std::vector<std::uint8_t> vegetal_map_data_glob(n*n);
    std::vector<std::uint8_t> fire_map_data_glob(n*n);

    // données du groupe de calcul
    int calcRank, calcSize;
    MPI_Comm_rank(calcComm, &calcRank);
    MPI_Comm_size(calcComm, &calcSize);

    while(simu_loc.update() && !stop){
        if ((rank == 1 && simu_loc.time_step() & 31) == 0)
          std::cout << "Time step " << simu_loc.time_step() << "\n===============" << std::endl;

      // échange ghost cells
      vegetal_map_data = simu_loc.vegetal_map();
      fire_map_data = simu_loc.fire_map();
      update_ghost_cells(vegetal_map_data, local_n, rank, size); 
      update_ghost_cells(fire_map_data, local_n, rank, size);
      // rassemble les données sur le processus 1/ le processus 0 de calcul
      // il faut prendre en compte l'offset
      // et le fait que la dernière tranche peut avoir une taille différente
      // seul le proc 1 en a besoin
      std::vector<int> counts_rev(calcSize);
      std::vector<int> displacements(calcSize);
      if (rank == 1){
        vegetal_map_data_glob.assign(n*n, 255);  // 255 = végétation par défaut
        fire_map_data_glob.assign(n*n, 0);  
        int offset = 0;
        int real_rows_number = 0;
        for(int i = 0; i < calcSize; i++){
          if(i == calcSize - 1){
            real_rows_number = n - rows_per_proc * (calcSize - 1);
          }
          else{
            real_rows_number = rows_per_proc;
          }
          // nombre d'elts à recevoir du proc i
          counts_rev[i] = real_rows_number * n;
          // décalage
          displacements[i] = offset;
          // on se souvient de l'offset pour la suite
          offset += real_rows_number * n;
        }
        // pas de ghost cells
        int sending_offset = 0;
        std::cout << "tout va bien" << std::endl;
        MPI_Gatherv(vegetal_map_data.data() + sending_offset, counts_rev[calcRank], MPI_UINT8_T,vegetal_map_data_glob.data(), counts_rev.data(), displacements.data(), 
                MPI_UINT8_T, 0, calcComm);
        std::cout << "alors peut être" << std::endl;
        MPI_Gatherv(fire_map_data.data() + sending_offset, counts_rev[calcRank], MPI_UINT8_T, fire_map_data_glob.data(), counts_rev.data(), displacements.data(), 
                MPI_UINT8_T, 0, calcComm);
        std::cout << "jures ??"<< std::endl;
      }
      // Autres processus
      else{
        // enlève les ghosts cells
        int sending_offset = n;
        int real_rows_number = local_n - 2;
        if (rank == calcSize - 1){
          real_rows_number = n - rows_per_proc * (calcSize - 1);
        }

        std::cout << "ici c'est of" << std::endl;
        MPI_Gatherv(vegetal_map_data.data() + sending_offset, real_rows_number * n, MPI_UINT8_T,nullptr, nullptr, nullptr, MPI_UINT8_T, 0, calcComm);
        std::cout << "rien à voir" << std::endl;
                
        MPI_Gatherv(fire_map_data.data() + sending_offset, real_rows_number * n, MPI_UINT8_T, nullptr, nullptr, nullptr, MPI_UINT8_T, 0, calcComm);
      }
      // offset de l'envoi
      // à par pour le proc 1, on saute les ghosts cells
      // int sending_offset = rank == 1 ? 0 :  n;

      
      // std::cout << "ok" << std::endl;
      // MPI_Gather(simu_loc.vegetal_map().data(), map_size, MPI_UINT8_T, vegetal_map_data_glob.data(), map_size, MPI_UINT8_T, 0, calcComm);
      // std::cout << "ok" << std::endl;

      // MPI_Gather(simu_loc.fire_map().data(), map_size, MPI_UINT8_T, fire_map_data_glob.data(), map_size, MPI_UINT8_T, 0, calcComm);

      if (rank==1){
      // envoie les données
        MPI_Send(vegetal_map_data_glob.data(), n*n, MPI_UINT8_T, 0, 0, globComm);
        MPI_Send(fire_map_data_glob.data(), n*n, MPI_UINT8_T, 0, 1, globComm);
        // vérifie si il faut arrêter
        MPI_Recv(&stop, 1, MPI_C_BOOL, 0, 2, globComm, MPI_STATUS_IGNORE);
        MPI_Send(&stop, 1, MPI_C_BOOL, 0, 3, globComm);
      }
      // envoyer les données
      // vegetal_map_data = simu.vegetal_map();
      // fire_map_data = simu.fire_map();
      // MPI_Send(vegetal_map_data.data(), map_size, MPI_UINT8_T, 0, 0, globComm);
      // MPI_Send(fire_map_data.data(), map_size, MPI_UINT8_T, 0, 1, globComm);
      // on vérifie si il faut arrêter
      // ce serait mieux d'utiliser de l'asynchrone, mais je n'ai pas réussi
      // MPI_Recv(&stop, 1, MPI_C_BOOL, 0, 2, globComm, MPI_STATUS_IGNORE);
      // MPI_Send(&stop, 1, MPI_C_BOOL, 0, 3, globComm);
    }
    // Afin d'arrêter l'affichage si le calcul est terminé
    // c'est laid mais ça marche
    if(!stop){
      MPI_Send(vegetal_map_data.data(), map_size, MPI_UINT8_T, 0, 0, globComm);
      MPI_Send(fire_map_data.data(), map_size, MPI_UINT8_T, 0, 1, globComm);
      MPI_Recv(&stop, 1, MPI_C_BOOL, 0, 2, globComm, MPI_STATUS_IGNORE);
      stop = true;
      MPI_Send(&stop, 1, MPI_C_BOOL, 0, 3, globComm);}
  }

  // libération du groupe de communication
  if (rank != 0)
    MPI_Comm_free(&calcComm);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
