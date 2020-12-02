//The code is supposed to be for 10000 nodes and 3000 edges ; but the program takes lot of time thus I have used smaller number here to get results early.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VERTICES 1000
// actually 10000
#define MAX_EDGES 300
// actually 3000
#define recovery_rate 0.2
#define transmission_rate 0.5
#define TMAX 300

typedef struct vertex {
    int* arrayData;// array to store neighbours of vertex
    char state;// state of the vertex
    int filled;// variable to store how many edges the vertex has
}vertex;

typedef struct system_node {
    int value;// stores value of a single type of list
    struct system_node* next;// doubly linked pointers
    struct system_node* previous;
}system_node;

typedef struct system_state {
    system_node*time;
    system_node*susceptible;
    system_node*infected;
    system_node*recovered;
}system_state;// struct to store lists of these four types of data

typedef struct events {
    int day_of_event;// day of event
    char type_of_event;// type of event 'T' for Transmission , 'R' for Recovery
    int node_index;//value of node concerned with the event
    struct events* next;// doubly linked queue
    struct events* previous;
}event;

event* enqueue(event* head,int day, char type,int node);// function to add event to priority queue
event* dequeue(event* head);// function to remove event with highest priority
void Print(event* head);// function to print the queue (days)
vertex ** createGraph(int numberOfVertices,int maxNumberOfEdges);// function to generate random undirected graph
int duplicateExists(vertex **graph,int source,int target);// function to checks if two vertices are linked or not
void printGraph(vertex **graph,int numberOfVertices);// function to print graphs
system_node* update_state(system_node* head,int value);// function to add new value to system state
void print_system_node(system_state * state);// function to print to terminal the entire data
void plot_system(system_state * state);// function to save to file to be used to create charts
int last_value(system_node*head);// function to return value from last added node insystem state list
system_state * process_trans_SIR(event* queue,vertex** graph,int source,int day,system_state * state);// function to process transmission
void find_trans_SIR(event* queue,vertex** graph,int neighbour,int day,int source);// function to find new transmissions
system_state * process_rec_SIR(vertex** graph,int source,int day,system_state * state);// function to process recovery


int main(void) {
    event* event_queue = NULL;// create the event queue
    srand ( time(NULL) );

    int numberOfVertices = rand() % MAX_VERTICES; //  number of maximum vertices
    int maxNumberOfEdges = rand() % MAX_EDGES;  //number of maximum edges a vertex can have

    system_state * state = NULL;// to store the state of system
    state=malloc(sizeof(system_state));
    state->infected=NULL;// default is NULL
    state->recovered=NULL;
    state->susceptible=NULL;
    state->time=NULL;
    /*Adding initial conditions*/
    state->infected=update_state(state->infected,0);
    state->recovered=update_state(state->recovered,0);
    state->susceptible=update_state(state->susceptible,numberOfVertices);
    state->time=update_state(state->time,0);
    /* creating the graph */
    vertex **graph = createGraph(numberOfVertices,maxNumberOfEdges);
    printf("\nVertices :%d , Maximum number of edges:%d\n",numberOfVertices,maxNumberOfEdges);
    //printGraph(graph,numberOfVertices); // uncomment to see all vertices
    int initial_infected = (MAX_VERTICES / 50) +1 ; // 2 percent of initial nodes (atleast 1) are initially infected

    for(int i = 0;i< initial_infected;i++)    {
        event_queue=enqueue(event_queue,0,'T',i);
    }// making the first few nodes because all are randomly linked

    while(event_queue!=NULL)  {// while event in queue
       char event_type = event_queue->type_of_event;// store type of event
        if(event_type=='T')  {// if event is transmission
            if(graph[event_queue->node_index]->state=='S')// and node is susceptible
                  process_trans_SIR(event_queue,graph,event_queue->node_index,event_queue->day_of_event+1,state); // process transmission
        }
        else if(graph[event_queue->node_index]->state=='I' && event_type=='R')  {// event is recovery
            process_rec_SIR(graph,event_queue->node_index,event_queue->day_of_event+1,state);
        }// process recovery
        event_queue=dequeue(event_queue);// dequeue processed event
    }
    //print_system_node(state);
    plot_system(state);// print to file the entire system state
}

system_state * process_trans_SIR(event* queue,vertex** graph,int source,int day,system_state * state)  {
    if(graph[source]->state=='S')  {// if node is susceptible
        graph[source]->state='I';// update state of node and of overall system
        state->time=update_state(state->time,day);
        state->susceptible=update_state(state->susceptible,last_value(state->susceptible)-1);
        state->infected=update_state(state->infected,last_value(state->infected)+1);
        state->recovered=update_state(state->recovered,last_value(state->recovered));
        int rec_time = day;// calculate recovery period
        int variate = rand()%10;// variable to store the probability of recovery
        while (variate>10*recovery_rate)  {
            rec_time++;// increase time of recovery until condition satisfied
            variate=rand()%10;
        }
        if(rec_time<TMAX)// if recovery time is before the end of required time
                queue=enqueue(queue,rec_time,'R',source);// add recovery event

        for(int neighbour = 0;neighbour< graph[source]->filled;neighbour++)  {
            find_trans_SIR(queue,graph,graph[source]->arrayData[neighbour],day,source);
        }// find transmission events for neighbours
    }
    return state;// return system after updating it
}

void find_trans_SIR(event* queue,vertex** graph,int neighbour,int day,int source)  {
    if(graph[neighbour]->state=='S')    {// if node was susceptible
        int inf_time = day;// calculate the infection period
        int variate = rand()%10;// variable to store the probability of infection
        while (variate>10*transmission_rate)  {
            inf_time++;// increase infection period
            variate=rand()%10;
        }
        if(inf_time<TMAX&&graph[source]->state=='I')// if infection period is before the end of required time
              queue=enqueue(queue,inf_time,'T',neighbour);// add transmission event to neighbour

    }
}

system_state * process_rec_SIR(vertex** graph,int source,int day,system_state * state)  {
    if(graph[source]->state=='I')  {// if node is infected
        graph[source]->state='R';// update state of node and of overall system
        state->time=update_state(state->time,day);
        state->susceptible=update_state(state->susceptible,last_value(state->susceptible));
        state->infected=update_state(state->infected,last_value(state->infected)-1);
        state->recovered=update_state(state->recovered,last_value(state->recovered)+1);
    }
    return state;
}

void print_system_node(system_state * state)   {
    while(state->time!=NULL)  { // while end of list is not reached print system's state to terminal
        printf("Day:%d Susceptible#:%d Infected#:%d Recovered#:%d\n",state->time->value,state->susceptible->value,state->infected->value,state->recovered->value);
        state->time=state->time->previous;
        state->susceptible=state->susceptible->previous;
        state->infected=state->infected->previous;
        state->recovered=state->recovered->previous;
    }
}
void plot_system(system_state * state)  {
    FILE* outPtr;
    outPtr=fopen("output.txt","w");
    while(state->time!=NULL)  { // while end of list is not reached print to text file
        fprintf(outPtr,"%d %d %d %d\n",state->time->value,state->susceptible->value,state->infected->value,state->recovered->value);
        state->time=state->time->previous;
        state->susceptible=state->susceptible->previous;
        state->infected=state->infected->previous;
        state->recovered=state->recovered->previous;
    }
    fclose(outPtr);
}

event* enqueue(event* head,int day, char type,int node)  {
    event* temp = head;
    if(temp!=NULL)  {
        while(temp->previous!=NULL&&day>=temp->previous->day_of_event)   {// until priority is greater than priority of new node traverse list
            temp=temp->previous;
        }
    }
    event* new_event= malloc(sizeof(event));// create memory for event and store required values
    new_event->day_of_event=day;
    new_event->type_of_event=type;
    new_event->node_index=node;
    if(temp!=NULL)  {// update next and previous pointers of adjacent nodes
        new_event->previous=temp->previous;
        new_event->next=temp;
        if(temp->previous!=NULL)
            temp->previous->next=new_event;

        temp->previous=new_event;
    }
    else  { // default ( empty ) case
        new_event->next=NULL;
        new_event->previous=NULL;
    }
    while(new_event->next!=NULL)  { // traverse back to head of list
        new_event=new_event->next;
    }
    return new_event;// return head
}

system_node* update_state(system_node* head,int value)  {
    system_node* temp = head;
    if(temp!=NULL)   { // go to end of queue
        while(temp->previous!=NULL)  {
            temp=temp->previous;
        }
    }
    system_node* state = malloc(sizeof(system_node));// create new node
    state->value=value;// store required value
    if(temp!=NULL)  { // update next and previous pointers of adjacent nodes
        state->previous=temp->previous;
        state->next=temp;
        if(temp->previous!=NULL)
            temp->previous->next=state;

        temp->previous=state;
    }
    else   {// default ( empty ) case
        state->next=NULL;
        state->previous=NULL;
    }
    while(state->next!=NULL)   {// traverse back to head of list
        state=state->next;
    }
    return state;// return head
}

int last_value(system_node*head)  {
    if(head!=NULL)  { // traverse to end of list
        while(head->previous!=NULL)  {
            head=head->previous;
        }
    }
    if(head!=NULL)// if value exists
        return head->value;// return last value

    else
        return 0;// return by default

}

void Print(event* head)  {
    while(head!=NULL)   {  // until end of queue is reached
        printf("%d ",head->day_of_event);// print day of event
        head=head->previous;// traverse the queue
    }
    printf("\n");
}

event* dequeue(event* head)  {
    event* temp = head->previous;// temporary store for new head
    free(head);// free memory at head
    if(temp!=NULL)// update next value at temp if temp exists
        temp->next=NULL;

    return temp;// retur new head
}

vertex ** createGraph(int numberOfVertices,int maxNumberOfEdges)  {
    /*graphs is 2 dimensional array of pointers*/
    if( numberOfVertices == 0)
        numberOfVertices++;

    vertex **graph;
    /*generate a dynamic array of random size*/
    if ((graph = (vertex **) malloc(sizeof(vertex *) * numberOfVertices)) == NULL) {
        printf("Could not allocate memory for graph\n");
        exit(1);
    }

    /*generate space for edges*/
    int vertexCounter = 0;
    /*generate space for vertices*/
    int edgeCounter = 0;
    for (vertexCounter = 0; vertexCounter < numberOfVertices; vertexCounter++)    {
        if ((graph[vertexCounter] = (vertex *) malloc(sizeof(vertex) * maxNumberOfEdges)) == NULL)  {
            printf("Could not allocate memory for edges\n");
            exit(1);
        }
        for (edgeCounter = 0; edgeCounter < maxNumberOfEdges; edgeCounter++)    {
            if ((graph[vertexCounter] = (vertex *) malloc(sizeof(vertex))) == NULL)  {
                printf("Could not allocate memory for vertex\n");
                exit(1);
            }
        }
        graph[vertexCounter]->filled=0;// fill default values
        graph[vertexCounter]->state='S';
        graph[vertexCounter]->arrayData=calloc(maxNumberOfEdges,sizeof(int));// alloacte memory for neighbours
    }
    /*start linking the graph. All vetrices need not have same number of links*/
    vertexCounter = 0;edgeCounter = 0;
    srand(time(NULL));
    for (vertexCounter = 0; vertexCounter < numberOfVertices; vertexCounter++)  {
        for (edgeCounter=0; edgeCounter < maxNumberOfEdges; edgeCounter++)    {
            int linkedVertex = rand() % numberOfVertices;
            if (rand()%2 == 1&&linkedVertex!=vertexCounter&&!duplicateExists(graph,vertexCounter,linkedVertex)&&graph[vertexCounter]->filled<maxNumberOfEdges&&graph[linkedVertex]->filled<maxNumberOfEdges)
            {
                // link from vertex a to b
                graph[vertexCounter]->arrayData[graph[vertexCounter]->filled]=linkedVertex;
                graph[vertexCounter]->filled+=1;
                // link from vertex b to a
                graph[linkedVertex]->arrayData[graph[linkedVertex]->filled]=vertexCounter;
                graph[linkedVertex]->filled+=1;
            }
        }
    }
    return graph;
}

int duplicateExists(vertex **graph,int source,int target)  {
    int* data_list = graph[source]->arrayData;// stores pointer to array of neighbours
    int size = graph[source]->filled;// stores the number of them
    for(int i = 0; i<size;i++)    {
        if(data_list[i]==target)// if duplicate exists
            return 1;// return true

    }
    return 0;// return false
}

void printGraph(vertex **graph,int numberOfVertices)  {
    for(int vertexCounter=0;vertexCounter<numberOfVertices;vertexCounter++)   { // for all source vertices
        printf("%d:",vertexCounter);// print source vertex value
        for(int edgeCounter=0;edgeCounter<graph[vertexCounter]->filled;edgeCounter++)  { // for all neighbour of source vertex
            printf("%d,",graph[vertexCounter]->arrayData[edgeCounter]);// print neighbour value
        }
        printf("\n");
    }
}
