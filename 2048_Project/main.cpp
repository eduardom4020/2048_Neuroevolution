#include <QCoreApplication>
#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include "game.h"
#include "genalgorithm.h"
#include "modnet.h"
#include "geneticinterface.h"
#include "individual.h"


#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // win32


//#define GAME_MAX_TIME       10
#define NUM_GENERATIONS     500
#define SIMULATION_VERSION  23

using namespace std;


void sleep(int milliseconds)
{
    #ifdef WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds * 1000);
    #endif // win32
}

void clear()
{
    #ifdef WIN32
    system("cls");  //windows
    #else
    system("clear");  //linux
    #endif // win32
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //temos dois parametros para executar a simulacao:
    //  -continue (continua a simulacao desde o ultimo individuo
    //da ultima geracao executada);
    //  -begin (inicia uma nova simulacao);
    //  -play (o melhor individuo da ultima geracao jogada eh
    //  carregado, exibindo entao uma partida, jogada por ele, ao usuario)

    /*primeiro vamos preparar o modelo de rede neural e o algoritmo genetico
     *para que possa ser usado no jogo.
     * Vamos usar 16 entradas, pois possuimos um tabuleiro 4x4. Alem disso
     * teremos 2 saidas, corespondentes aos nossos comandos.
     */
    //cada rede neural serah chamada de individou(individual):
    NeuromodulatedNetwork individual;

    //16 entradas:
    for(int i=0; i<16; i++)
        individual.addCell(ntAfferent);
    //2 saidas, pois temos 4 comandos:
    for(int i=0; i<2; i++)
        individual.addCell(ntEfferent);

    //agora criamos nosso algoritmo genetico, que serah chamado de evol:
    GeneticAlgorithm evol;

    /*o objeto abaixo faz a interacao entre o algoritmo genetico e os cerebros,
     *alem de nos permitir ligar os outputs nos comandos do jogo. Ao final de
     * cada partida devemos calcular a avaliacao daquele cerebro.
    */
    GeneticInterface g_interface;

//declaracao de variaveis que nao fazem parte do algoritmo genetico:
//============================================================================================
    Game game;      //criamos um objeto do tipo jogo que serah utilizado pelos individuos
    bool error;
    float p = 0.0;

    ostringstream checkpoint;
    checkpoint << "checkpoint_" << SIMULATION_VERSION;
//============================================================================================

//utilizamos os parametros entrados para sabermos quais acoes devem ser tomadas pelo programa:
    if(strcmp(argv[1], "-begin")==0)
    {

    /* ("cromossomo1;cromossomo2;...;cromossomoN", numero de inviduos na população do algortimos genetico (no de cerebros))
     *f = tamanho fixo
     *r = tamanho aleatorio de (min,max)
     *nesse caso todo individuo tem2 cromossomos um de tam 6 e outro variavel de (10,100) em bits (int = 32bits, independente da maquina)
     *f(192) = 192/32 = 6 genes fixos (configuracao geral)
     *r(min,max) indica quantos genes serao criados aleatoriamente, podendo ser neuronio ou coneccao
    */
        evol.generateRandomPopulation("f(192);f(8000);", 100);

        clear();
        cout << "Comecando um novo jogo!" << endl;

        sleep(1000);
    }

    else if (strcmp(argv[1], "-continue")==0)
    {
    //para este caso, devemos carregar a geracao anterior feita pelo algoritmo

        evol = GeneticAlgorithm(checkpoint.str().c_str());
    }

    /*agora faremos o loop em que a evolucao ocorrerah, ou seja, para cada geracao
     *cada individuo jogarah uma partida e serah avaliado. Ao final dessas avaliacoes
     *o algoritmo evolui os individuos e avanca uma geracao. Ao final das geracoes
     *deveremos ter o melhor individuo.
    */

    while(evol.getGeneration() < NUM_GENERATIONS)   //loop das geracoes
    {
        ostringstream score_txt;
        score_txt << "scores_simulation_" << SIMULATION_VERSION <<".txt";

        ostringstream gs_txt;
        gs_txt << "greater_square_simulation_" << SIMULATION_VERSION << ".txt";

        ostringstream eval_txt;
        eval_txt << "eval_simulation_" << SIMULATION_VERSION <<".txt";

        ofstream file_writer;

        clear();
        cout<<"Geracao: "<< evol.getGeneration()<<endl;

        for(unsigned int i=0; i<evol.getPopulationSize(); i++)  //loop dos individuos
        {
//            clear();
//            cout<<"Geracao: "<< evol.getGeneration()<<endl
//                <<"Individuo: "<<i<<endl;

//            time_t start = time(NULL);

            //o individuo i eh entao traduzido para a codificacao das redes neurais feita pelo Yuri
            g_interface.setPhenotype(evol.getIndividual(i), individual);

            game.initGame();

            while(game.gameIsNotOver)
            {
//                printf("Tempo: %d\n", (time(NULL) - start));
//                game.showGame();

                //adicionamos os tiles nomalizados, para facilitar o aprendizado do agente
                individual.setInput(game.getGameState());
                individual.process(p);

                //vamos entao conferir se a rede conseguiu gerar duas saidas, se sim vamos processa-las como comando
                if(individual.getNumberOfOutputs()==2)
                {
                    error = false;
                    floatv output = individual.getOutput();

                    if((output[0]<0)&&(output[1]<0))  //comando esquerda
                        game.setInput(4);
                    else if((output[0]<0)&&(output[1]>=0))  //comando acima
                        game.setInput(8);
                    else if((output[0]>=0)&&(output[1]<0))  //comando direita
                        game.setInput(6);
                    else if((output[0]>=0)&&(output[1]>=0))  //comando abaixo
                        game.setInput(2);             
                }
                else
                {
                //ao surgir um individuo com o numero de neuronios de saida sendo zero
                //o individuo eh zerado, segundo a propria implementacao do individuo.
                //Precisamos entao redefinir o numero de entradas e de saidas.

                //Ou seja, o comando addCell cria uma mascara que determina quantos
                //neuronis serao necessarios para que o individuo seja interpretado
                //quando um individuo possui menos neuronios que os exigidos para
                //E/S, esta mascara eh deletada. Com isso precisamos redefinir a quantidade
                //de neuronios, recriando a mascara que foi deletada.

                    for(int i=0; i<16; i++)
                        individual.addCell(ntAfferent);
                    for(int i=0; i<2; i++)
                        individual.addCell(ntEfferent);

                    error = true;
                    clear();
                    cout<<"Individuo "<<i<<"possui cerebro incompativel!"<<endl;

                    evol.setEvaluation(i,0); // -1 deve ser um valor muito ruim para que esse individuo nao se repita
                    game.score = -1;
                    game.gameIsNotOver = false;

//                    sleep(500);
                }

            }
            /*avaliamos entao o individuo que acabou de jogar a partida, para isso usaremos a seguinte funcao:
            * A pontuacao maxima a ser obtida em uma partida de 2048 eh de 3,932,156.
            *
            * Entao uma boa funcao de avaliacao seria o score chegar a este valor, como a avaliacao varia
            * entre 0 e 1, faremos uma funcao normalizada:
            *
            * score/3,932,156. Quanto mais proximo de um, mais adaptado estarah o individuo
            *
            */

            float evaluation = game.getScore()/36860.0/*3932156.0*/; //score para chegar ao 2048
//            cout<<"Avaliacao: "<< evaluation << endl;
//            sleep(500);

            if(!error)
                evol.setEvaluation(i,evaluation);


            file_writer.open(score_txt.str().c_str(), ios_base::out | ios_base::app);
            file_writer << game.getScore() << " ";
            file_writer.close();

            file_writer.open(gs_txt.str().c_str(), ios_base::out | ios_base::app);
            file_writer << game.getGreaterSquare() << " ";
            file_writer.close();

            file_writer.open(eval_txt.str().c_str(), ios_base::out | ios_base::app);
            file_writer << evaluation << " ";
            file_writer.close();

        }

        file_writer.open(score_txt.str().c_str(), ios_base::out | ios_base::app);
        file_writer << endl;
        file_writer.close();

        file_writer.open(gs_txt.str().c_str(), ios_base::out | ios_base::app);
        file_writer << endl;
        file_writer.close();

        file_writer.open(eval_txt.str().c_str(), ios_base::out | ios_base::app);
        file_writer << endl;
        file_writer.close();

        //ao final do loop dos individuos, basta evoluir a geracao e salva-la:
        evol.evolve();
        evol.saveCheckPoint(checkpoint.str().c_str());
    }

    return 0;
}
