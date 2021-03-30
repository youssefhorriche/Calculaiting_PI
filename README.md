# Calculaiting_PI



	Erklärung des Algorithmus

1	Leibniz Reihe Algorithmus 

Mit die Leibniz-Reihe lässt sich die Kreiszahl PI einfach und übersichtlich berechnen.


 


Den Algorithmus konvergiert immer weiter zu Pi-Viertel, und zwar indem abwechslungsweise ein Teil weg und wieder dazu gerechnet wird. Diese Teile werden immer kleiner und so nähert sich das Ergebnis immer weiter an Pi-Viertel. Der Nachteil dieses Algorithmus sind die vielen Iterationen welche nötig sind, um eine tiefe Genauigkeit von Pi zu erhalten.


2	Monte-Carlo Algorithmus

Mit dem Namen Monte-Carlo bzw. Monte-Carlo-Simulation verbindet man die Lösung von
mathematischen Problemstellungen mit Hilfe von Zufallszahlen.
für die Berechnung der Kreisfläche oder auch der Zahl Pi beginnen wir mit einem Quadrat der
Fläche 1. Dieses Quadrat hat die Kantenlänge 1.
In dieses Quadrat zeichnen wir einen Viertelkreisbogen mit dem Radius 1 ein.
Wir erzeugen mit einem Zufallsgenerator beliebige Punkte innerhalb des Quadrats. Das bedeutet,
dass die Punkte innerhalb des Quadrats jeweils x- und y-Werte im Bereich von 0 bis 1 haben.
Bei mehreren tausenden solcher Punkte füllt sich das Quadrat mehr oder weniger gleichmäßig mit
diesen Punkten.
