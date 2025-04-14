/****************************
 *                          * 
 *   Autor: Adam Behoun     *  
 *    IZP - 2. projekt      *
 *   Datum: 28.11.2023      *
 *                          * 
 ****************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define TRIANGLE_BORDERS 3
#define LEFT 1
#define RIGHT 2
#define HORIZ 4

// ENUMS
enum rules { L = -1, R = 1 };
enum orientations { NORMAL, UPSIDE_DOWN };
enum arguments { HELP=2, TEST, PATH };

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

Map* map_ini(int rows, int cols);
Map* read_map(char *filename);
int start_border(Map *map, int r, int c, int leftright);
void map_free(Map *map);
bool isborder(Map *map, int r, int c, int border); 
int start_orientation(int start_row, int start_col, Map *map);
int foo_main(Map* map, int curr_row, int curr_col, int rule);
int modulo(int a, int b);
int valid_arguments(int argc, char *argv[]);
bool valid_file(char *filename);
int return_path(char *argv[]);
bool border_cmp(int val1, int val2, int border1, int border2);
void print_help();
 
int main(int argc, char *argv[]) 
{
    int test_arguments = valid_arguments(argc, argv);
    if(test_arguments == EXIT_FAILURE){
        fprintf(stderr, "Program byl nespravne spusten. Pro napovedu pouzijte -> './maze --help' \n");
        return 2;
    } else if (test_arguments == HELP) {
        print_help();
    } else if (test_arguments == TEST){
        bool valid = valid_file(argv[2]);
        if(valid){
            printf("Valid\n");
        } else {
            printf("Invalid\n");
        }
    } else {
        if(!valid_file(argv[4])){
            fprintf(stderr, "Soubor s bludistem, neni validni. Spustte './maze --test SOUBOR', pro zkousku validace.\n");
            return 3;
        }

        Map* mp = read_map(argv[4]);

        if(mp == NULL || mp->cells == NULL){
            fprintf(stderr, "Nepodarilo se alokovat pamet pro strukturu.\n");
            return 4;
        }

        int start_rows = atoi(argv[2]);
        int start_cols = atoi(argv[3]);

        int path_value = return_path(argv);
    
        int main_function = foo_main(mp, start_rows, start_cols, path_value);

        map_free(mp);

        if(main_function == EXIT_FAILURE){
            fprintf(stderr, "Nepodarilo se najit hranici na vstup\n");
            return 5;
        }
    }

	return 0;
}

/**
 * @brief Initialize the map data structure
 *
 * @param rows number of rows of the maze
 * @param cols number of cols of the maze
 * @return Map*
 */
Map* map_ini(int rows, int cols) {
    Map* result = malloc(sizeof(Map));

    if(result == NULL){
        return result;
    }

    result->rows = rows;
    result->cols = cols;

    result->cells = malloc(result->cols * result->rows * sizeof(unsigned char));

    if(result->cells == NULL){
        free(result);
        return result;
    }

    return result;
}

/**
 * @brief Load the map from the file
 * 
 * @param filename 
 * @return Map*
 */
Map* read_map(char *filename){
    Map* map = NULL;

    FILE* ptr = fopen(filename, "r");
    if(ptr == NULL){
        return map; 
    }

    // first row contains two numbers - rows and cols
    int rows, cols;
    fscanf(ptr, "%d %d", &rows, &cols);

    map = map_ini(rows, cols);

    if(map == NULL || map->cells == NULL) {
        fclose(ptr);
        return map;
    }

    for(int i = 0; i < rows*cols; i++){
        fscanf(ptr, "%hhd", &map->cells[i]);
    }

    fclose(ptr);
    return map;
}

/**
 * @brief free the map and its cells if they are not empty
 * 
 * @param map
 */
void map_free(Map *map){
    if(map != NULL) {
        if(map->cells != NULL)
            free(map->cells);
        free(map);
    }
}

/**
 * @brief return the border where to start by the assignment rules
 * 
 * @param map
 * @param r row
 * @param c column
 * @param leftright rule to use
 */
int start_border(Map *map, int r, int c, int leftright) {

    if((r == 1 || r == map->rows) && c%2 == 0 && c != map->cols){
        return -1;
    } else if (r > 1 && c > 1 && c < map->cols && r < map->rows) {
        return -1;
    } 
    
    if(leftright == R) {
        if(r%2 != 0 && c == 1 && !isborder(map, r, c, LEFT))
           return RIGHT; 
        else if (r%2 == 0 && c == 1 && !isborder(map, r, c, LEFT))
            return HORIZ;
        else if (c == map->cols && (r+c) % 2 == 0 && !isborder(map, r, c, RIGHT)) 
            return HORIZ;
        else if (c == map->cols && (r+c) % 2 != 0 && !isborder(map, r, c, RIGHT)) 
            return LEFT;
        else if (r == 1 && !isborder(map, r, c, HORIZ)) 
            return LEFT;
        else if (r == map->rows && !isborder(map, r, c, HORIZ)) 
            return RIGHT; 
    } else if (leftright == L) {
        if(r%2 != 0 && c == 1 && !isborder(map, r, c, LEFT)) 
            return HORIZ;
        else if (r%2 == 0 && c == 1 && !isborder(map, r, c, LEFT)) 
            return RIGHT;
        else if (c == map->cols && (r+c) % 2 != 0 && !isborder(map, r, c, RIGHT)) 
            return HORIZ;
        else if (c == map->cols && (r+c) % 2 == 0 && !isborder(map, r, c, RIGHT)) 
            return LEFT;
        else if (r == 1 && !isborder(map, r, c, HORIZ)) 
            return RIGHT;
        else if (r == map->rows && !isborder(map, r, c, HORIZ)) 
            return LEFT;
    }
    return -1;
}

/**
 * @brief calculate, if there is a border on the position
 * 
 * @param map 
 * @param r row
 * @param c column
 * @param border to find
 * @return true 
 * @return false
 */
bool isborder(Map *map, int r, int c, int border) {
    // use the equation
    int position = (((r-1)*map->cols) + c)-1;

    if(map->cells[position] & border)
        return true;

    return false;
}

/**
 * @brief Return the orientation of a triangle at the start
 * 
 * @param start_row
 * @param start_col
 * @param map
 * @return UPSIDE_DOWN - vertical at the top NORMAL - vertical at the bottom
 */
int start_orientation(int start_row, int start_col, Map *map){
    if(map->cols % 2 == 0) {
        if(start_row % 2 != 0 && start_col % 2 != 0 && start_col != map->cols) {
            return UPSIDE_DOWN;
        } else if(start_row % 2 == 0 && start_col == map->cols) {
            return UPSIDE_DOWN;
        } else {
            return NORMAL;
        }
    } else {
        if(start_row % 2 != 0 && start_col % 2 != 0) {
            return UPSIDE_DOWN;
        } else {
            return NORMAL;
        }
    }
}

int modulo(int a, int b) {
    return ((a % b) + b) % b;
}

int foo_main(Map *map, int curr_row, int curr_col, int rule) {

    int orientation = start_orientation(curr_row, curr_col, map);

    int first_border = start_border(map, curr_row , curr_col, rule);

    if(first_border == -1){
        return EXIT_FAILURE;
    }

    int borders[TRIANGLE_BORDERS] = {LEFT,RIGHT,HORIZ}; // LEFT[0], RIGHT[1], HORIZ[2] 

    int border_index;
    for(int i = 0; i < TRIANGLE_BORDERS; i++){
        if(first_border == borders[i])
            border_index = i;
    }
    
    bool first_iteration = true;

    // until not out of the bounds of the maze
    while(curr_row > 0 && curr_col > 0 && curr_row <= map->rows && curr_col <= map->cols){
        printf("%d,%d\n", curr_row, curr_col); 
        bool found = false;
        int index = modulo(border_index, TRIANGLE_BORDERS);

        int diff = (orientation % 2 == NORMAL) ? -1 : 1;
        diff *= rule;

        while(!found){

            int index_of_curr = (first_iteration) ? modulo(index,TRIANGLE_BORDERS) : modulo(index+diff, TRIANGLE_BORDERS);

            if(!isborder(map, curr_row, curr_col, borders[index_of_curr])){

                // crossing left border
                if(index_of_curr == 0) {
                    curr_col --;
                    border_index = modulo(index_of_curr + 1, TRIANGLE_BORDERS);

                // crossing right border
                } else if (index_of_curr == 1) {
                    curr_col ++;
                    border_index = modulo(index_of_curr - 1,TRIANGLE_BORDERS);

                // crossing bottom border
                } else if (index_of_curr == 2 && orientation % 2 == NORMAL) {
                    curr_row ++;
                    border_index = index_of_curr;

                } else {
                    curr_row --;
                    border_index = index_of_curr;
                }

                found =true;

            } else {
                index = (first_iteration) ? index : index+diff;
            }
            first_iteration = false;
        }
        orientation ++;
    }
    
    return EXIT_SUCCESS;
}

/**
 * @brief Validate the command-line arguments
 * 
 * @param argc number of the arguments
 * @param argv arguments themselves
 */
int valid_arguments(int argc, char *argv[]) {
    if(argc == 2 && strcmp(argv[1], "--help")==0){
        return HELP;
    } else if (argc == 3 && strcmp(argv[1], "--test")==0) {
        return TEST;
    } else if (argc == 5 && (strcmp(argv[1], "--lpath")==0 || strcmp(argv[1], "--rpath")==0)){
        return PATH;
    } else {
        return EXIT_FAILURE;
    }
}

bool border_cmp(int val1, int val2, int border1, int border2){

    if(border1 == RIGHT){
        bool current = val1 & border1;
        bool next = val2 & border2;
        if(current != next)
            return false;
    } else {
        bool current = val1 & border1;
        bool next = val2 & border2;
        if(current != next)
            return false;
    }
    return true;
}

/**
 * @brief Validate the maze in the file
 * 
 * @param filename the file containing the maze
 */
bool valid_file(char *filename) {
    FILE* ptr;
    ptr = fopen(filename, "r");

    if(ptr == NULL) {
        return false;
    }

    int typed_rows = 0, typed_cols =0;

    if((fscanf(ptr, "%d %d", &typed_rows, &typed_cols)) != 2) {
        fclose(ptr);
        return false;
    }

    // the number of the cells it should be by the specified two numbers at the beggining
    int desired_cells = typed_rows * typed_cols;

    int num[typed_rows *typed_cols];
    int matrix_num = 0;
    while(fscanf(ptr, "%d", &num[matrix_num]) == 1){
        if(num[matrix_num] >= 0 && num[matrix_num] <= 7)
            matrix_num ++;

        if(matrix_num >= typed_rows * typed_cols)
            break;
    }

    if(desired_cells != matrix_num){
        fclose(ptr);
        return false;
    }

    // Test na to, jestli se nevylucuji hranice policek  

    int curr_row = 0, curr_col = 0;

    for(int i = 0; i < typed_cols * typed_rows; i++){
        // Kdyz radky + sloupce % 2 == 0, tak je trojuhelnik otoceny 'vzhuru nohama', jinak je orientovan normalne
        int orientation = (curr_row + curr_col) % 2 == 0 ? UPSIDE_DOWN : NORMAL;

        // check if the right border of the current triangle is the same as the left border of 
        // the triangle below or to the right
        if(curr_col < typed_cols-1) {
            if(!border_cmp(num[curr_row * typed_cols + curr_col], num[curr_row * typed_cols + curr_col + 1], RIGHT, LEFT)){
                fclose(ptr);
                return false;
            }
        }

        // check if the bottom border of the current triangle is the same as the top border of 
        // the triangle below
        if(orientation == NORMAL && curr_row != typed_rows-1){
            if(!border_cmp(num[curr_row * typed_cols + curr_col], num[(curr_row+1) * typed_cols + curr_col], HORIZ, HORIZ)){
                fclose(ptr);
                return false;
            }
        } 

        curr_col ++;

        if(curr_col >= typed_cols){
            curr_row++;
            curr_col = 0;
        }
    }
    fclose(ptr);

    return true;
}

/**
 * @brief return the path to take
 */
int return_path(char *argv[]) {
    if(strcmp(argv[1], "--rpath")==0){
        return R;
    } else {
        return L;
    }
}

/**
 * @brief print the help
 * 
 */
void print_help(){
    const char *help =  " Pouziti: ./maze --help\n"
                    "          ./maze --test SOUBOR\n"
                    "          ./maze <prikaz> R S SOUBOR\n"
                    "\n"
                    " Prikazy:\n"
                    "   --help       Vypise jak ma vypadat spravny syntax spusteni souboru.\n"
                    "   --test       Zkontroluje, ze soubour obsahuje radnou definici mapy bludiste.\n"
                    "                V pripade, ze je format v poradku, vytiske Valid.\n" 
                    "                V opacnem pripade tiskne Invalid.\n"
                    "   --rpath      Hleda pruchod bludistem ze souboru pocinajici na radku R a sloupci S.\n"
                    "                Ridi se pravidlem prave ruky (prava ruka vzdy na zdi).\n"
                    "   --lpath      Hleda pruchod bludistem ze souboru pocinajici na radku R a sloupci S.\n"
                    "                Ridi se pravidlem leve ruky (leva ruka vzdy na zdi).\n"
                    "\n"
                    " Argumenty:\n"
                    "   R            Hodnota radku, na kterem se bude zacinat hledat cesta bludistem.\n"
                    "   S            Hodnota sloupce, na kterem se bude zacinat hledat cesta bludistem.\n"
                    "   SOUBOR       Nazev souboru, ze ktereho bude program cist mapu bludiste.\n";
    printf("%s", help);
}
