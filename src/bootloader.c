

//asm (".global __reset\n"
//     "__reset: "
//     "BRA _boot");

asm (
".global __boot         \n"
"__boot:                \n"
"BRA __reset            \n"
);


int bootloader_main() {
    // First, disable WDT — it will obstruct our operations
}
