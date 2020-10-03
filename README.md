# DESCrack
Progetto di fine corso per "Programmazione di Sistemi Multicore", presso l'università La Sapienza, a.a. 2019-2020. \
\
<em>DESCrack</em> è un programma che, tramite una ricerca esaustiva, cerca la chiave di un blocco cifrato con DES. \
Può girare su CPU (su un singolo core, oppure su più core usando OpenMP o MPI) oppure su GPU (usando CUDA). \
Questo repository contiene anche <em>DESEncrypt</em>, un programma per generare un blocco cifrato con DES.

# Dipendenze
* `CMake >= 3.18` per generare i makefile
* `OpenMP` per compilare ed eseguire `descrack-openmp` (opzionale)
* `OpenMPI` per compilare ed eseguire `descrack-mpi` (opzionale)
* `CUDA Toolkit` per compilare ed eseguire `descrack-cuda` (opzionale)

# Configurazione
`cmake . -B build -D CMAKE_CUDA_ARCHITECTURES=<arch>` dove `<arch>` è l'architettura CUDA utilizzata

# Compilazione
`make -C build -sj <n>` dove `<n>` è il numero di jobs simultanei

# Uso (descrack\*)
`descrack* [-m <n>] [-M <n>] [-a <alfabeto>] [-x] [-s] <blocco-cifrato> <testo-in-chiaro>`

Argomento | Descrizione | Valori ammessi
--------- | ----------- | --------------
`<blocco-cifrato>` | blocco da decifrare | una stringa di 16 cifre esadecimali (come l'output di `desencrypt`)
`<testo-in-chiaro>` | testo in chiaro | una stringa di 8 caratteri ASCII (o 16 cifre esadecimali, se `x` è abilitato)

Opzione | Descrizione | Valore di default | Valori ammessi
------- | ----------- | ----------------- | -------------
`m` | lunghezza minima della chiave da cercare | 1 | un numero tra 1 e 8
`M` | lunghezza massima della chiave da cercare | 8 | un numero tra 1 e 8
`a` | alfabeto da utilizzare per limitare la ricerca | `dlus` | una stringa composta da: `d` (cifre), `l` (minuscole), `u` (maiuscole), `s` (simboli), `c` (caratteri di controllo)
`x` | tratta `<testo-in-chiaro>` come valore esadecimale | disabilitato |

# Uso (desencrypt)
`desencrypt [-x] [-X] [-s] <testo-in-chiaro> <chiave>`

Argomento | Descrizione | Valori ammessi
--------- | ----------- | --------------
`<testo-in-chiaro>` | testo da cifrare | una stringa di 8 caratteri ASCII (o 16 cifre esadecimali, se `x` è abilitato)
`<chiave>` | chiave da utilizzare | una stringa di 8 caratteri ASCII (o 16 cifre esadecimali, se `X` è abilitato)

Opzione | Descrizione | Valore di default
------- | ----------- | -----------------
`x` | tratta `<testo-in-chiaro>` come valore esadecimale | disabilitato
`X` | tratta `<chiave>` come valore esadecimale | disabilitato
`s` | applica un left shift alla chiave (vedi nota in basso) | disabilitato

# Note sull'opzione `s`
DES utilizza il LSB di ogni byte della chiave come bit di parità (dispari). In caso di chiavi composte da soli caratteri ASCII (stampabili o meno), lo spazio di ricerca si riduce da 2<sup>56</sup> a 2<sup>48</sup>, per via del MSB (`0`) di ogni carattere. \
L'opzione `s` applica un left shift di una posizione alla chiave, "spostando" questi bit fissi nella posizione dei bit di parità.
