#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

char *errorBanner[] = {
    "",
    "    Error"};

// Option to horizontal menu
char *oneOption[] = {
    "Aceptar"};

// Options to horizontal menu
char *twoOptions[] = {
    "Aceptar",
    "Cancelar"};

char *newMenuUnlocked[] = {
    "   Nuevo menu",
    "  desbloqueado!"};

/*************** Level welcome messages ************************/

// LAYER LEVEL 2
char *bannerLevel2[] = {
    "****************",
    "***  Nivel 2 ***",
    "****************"};

// LAYER LEVEL 3
char *bannerLevel3[] = {
    "****************",
    "***  Nivel 3 ***",
    "****************"};

// LAYER WELCOME LEVEL 2
char *welcomeLevel2[] = {
    "Bienvenido al   ",
    "mundo de Harald,",
    "dile al rey la  ",
    "fecha correcta. ",
    "Puedes usar tu  ",
    "movil para      ",
    "conectarte      ",
    "conmigo ;)      ",
    "TIP: revisa tu  ",
    "direccion MAC   ",
    "en propiedades. ",
    "Te veo en el    ",
    "SC CONTROL POINT"};

// LAYER WELCOME LEVEL 3
char *welcomeLevel3[] = {
    "Tardaste en     ",
    "llegar. Has oido",
    "del I2C? del    ",
    "UART? del CAN?  ",
    "del IoT? jajaja ",
    "por TODOS lados ",
    "hasta por los   ",
    "GPIOS...        ",
    "Ven, acercate,  ",
    "tengo algo que  ",
    "contarte... pero",
    "es muuuy secreto",
    "Ay no se donde  ",
    "deje la llave   "};

/*************** Level finish messages ************************/

// LAYER FINISH LEVEL 1
char *finishLevel1[] = {
    "Has logrado el  ",
    "primer reto...  ",
    "en que momento  ",
    "inicio esto??   ",
    "Es solo un      ",
    "numero de 4     ",
    "digitos, nos    ",
    "vemos en el     ",
    "siguiente nivel ",
    "BLE BLE BLE     ",
    "Por cierto, si  ",
    "sigues buscando ",
    "conexiones      ",
    "tenemos algo    ",
    "que tal vez     ",
    "te interese...  "};

// LAYER FINISH LEVEL 2
char *finishLevel2[] = {
    "Excelente, pense",
    "que no lo ibas a",
    "lograr, es      ",
    "broma... La     ",
    "siguiente no es ",
    "facil, prepara  ",
    "el hardware...  "};

/************************ Main menu ***************************/

char *mainOptions[] = {
    "1. LEDs",
    "2. Niveles",
    "3. Amigos",
    "4. Propiedades",
    "5. Sensor",
    "6. Secreto"};

/************************ LEDs menu ***************************/

char *neopixelsOptions[] = {
    "1. LED 1",
    "2. LED 2",
    "3. LED 3",
    "4. Modo arcoiris"};

char *neopixelOptions[] = {
    "1. Rojo",
    "2. Verde",
    "3. Azul",
    "4. Encender",
    "5. Apagar"};

/*********************** Levels menu **************************/

// LAYER_LEVELS_MENU
char *levelsMenu[] = {
    "1. Nivel 1",
    "2. Nivel 2",
    "3. Nivel 3"};

// LAYER_LEVEL_1_OPTONS, LAYER_LEVEL_2_OPTONS, LAYER_LEVEL_3_OPTONS
char *levelsOptions[] = {
    "1. Ayuda"};

char *level1Help[] = {
    "",
    "Consigue amigos "};

/*********************** Friends menu *************************/

char *friendOptions[] = {
    "1. Amigos: ",
    "2. Buscar",
    "3. Ayuda"};

char *friendSearch[] = {
    "",
    "  Buscando..."};

// Friends help text
char *friendHelp[] = {
    " Acercate a un",
    "   amigo para",
    "   agregarlo"};

char *friendFoundBanner[] = {
    "     Amigo",
    "   encontrado"};

/********************** Properties menu ***********************/

char *properties[] = {
    "1. Creditos     ",
    "2. Chip:CH32V208",
    "3. MAC:",
    "",
    "4. Codigo:000000"};

char *credits[] = {
    "Desarrollado por",
    "Electroic Cats, ",
    "Agradecemos a:  ",
    "EkoParty Team,  ",
    "gracias por     ",
    "creer en el     ",
    "proyecto.       ",
    "Firmware:       ",
    "Francisco -     ",
    "@DeimosHall     ",
    "Hardware:       ",
    "Lizeth          ",
    "Los que nos     ",
    "ayudaron a hacer",
    "llegar el       ",
    "hardware a      ",
    "Argentina       ",
    "#QueVuelvaLa    ",
    "BarraEnLaEko    ",
    "                ",
    "La Llave siempre",
    "    ha sido:    ",
    " TEAMOEKOPARTY  "};

/*********************** Sensor menu **************************/

char *sensorMenu[] = {
    "1. Ingresar",
    "2. Ayuda"};

// LAYER SENSOR QUESTION
char *sensorQuestion[] = {
    "",
    "   Dato:"};

// LAYER WRONG YEAR
char *wrongYear[] = {
    "",
    "   Incorrecto"};

// LAYER CORRECT YEAR
char *correctYear[] = {
    "",
    "    Correcto"};

// LAYER SENSOR HELP
char *sensorHelp[] = {
    "     No hay     ",
    "      ayuda     "};

/*********************** Secret menu **************************/

// LAYER SECRET MENU
char *bannerSecret[] = {
    "",
    " Escuchando...  "};

#endif