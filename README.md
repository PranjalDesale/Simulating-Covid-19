# Simulating-Covid-19

1. In this project, I performed a simulation similar to the Covid-19 pandemic. The simulation was created using C language and for Python was used for representation of execution of simulation and the final output. 
2. Simulation is performed for a maximum of 300 days, with recovery-rate 0.2 and transmission-rate 0.5 
3. The structure includes construction of a graph of 10000 nodes which represent number of people and randomly created 3000 undirected edges that represents dual path contact within the people.
4. An epidemic like Covid-19 is an SIR epidemic. An individual is initially Susceptible (S). Then s/he might get Infected (I) and finally Recover (R). However,as one can expect, each transition is probabilistic.
5. I have implemented a simple variant of the algorithm in section of A.1.2 of:https://link.springer.com/content/pdf/bbm%3A978-3-319-50806-1%2F1.pdf 
