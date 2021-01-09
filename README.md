# UOLL-I

Questo progetto è stato realizzato in collaborazione con altri tre studenti per il corso di Embedded System tenuto all’università degli studi di Salerno. Lo scopo del progetto consiste nell’utilizzare il microcontrollore STM32 donatoci dall’università per realizzare un embedded system. Ispirati dal film WALL-E e dall’omonimo robot, è stato realizzato un sistema dotato di braccia e ruote cingolate capace di raccogliere all’interno del suo corpo gli oggetti che si trova di fronte.

Il sistema è quindi caratterizzato da molti componenti di cui si presenta lo schema di seguito. Sono stati utilizzati diversi sensori, come il sensore di peso HX711, o il sensore di profondità a ultrasuoni HC-SR04. Sono inoltre presenti cinque motori Servo LD20 per il movimento delle braccia e della panca e due motori brushed GM25-370 per il movimento del cingolato. 
Ogni comando può essere impartito al robot tramite un’applicazione mobile che si interfaccia con il microcontrollore attraverso un modulo Bluetooth HC-06 seguendo un protocollo seriale UART. Infine è stato utilizzato il protocollo I^2 C ed il relativo modulo per l’utilizzo del display LCD.


