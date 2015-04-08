
ADSP-BF561:  Blackfin Symmetric Multi-Processor for Consumer Multimedia 
http://www.analog.com/en/processors-dsp/blackfin/adsp-bf561/products/product.html


Port: Georg.Raith@dlr.de
Juni 2014


    Context Switch
    Startup Code
    Speicherverwaltung
    DMA
    Cache/MPU
    Interruptservices
    stdoutput
    Core Timer
    Mail Box (Interrupt save)
    Fifo (Interrupt save)
    Queue (Interrupt save)
    Linkerscripte

Die aktuelle Softwarestand ist noch eine Vorabversion, die sich in Tests aber als lauffähig und stabil gezeigt hat. Wir haben bei unserer Implementierung viel Wert auf Interruptsichere Übergänge gelegt, da die Hardware über einen FPGA verfügt, welcher über Interrupts kommuniziert. Aus diesem Grunde fehlen uns einige Funktionen die die Interaktion zwischen Interrupts und Threads verbessern würden.
