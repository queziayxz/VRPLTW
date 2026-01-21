## Arrumar
- Adicionar o custo de atribuição no locker (de acordo com a distância)
- Limitar os lockers que podem ser atribuidos ao cliente, de acordo com o radius da instância

## interessante
- deixar o mutation_rate dinâmico, igual o SA faz, calcula a probabilidade de mudança de acordo com a temperatura

## Obs
- Em todos as instâncias, terão os mesmos lockers mas em quantidade diferente
- Não possui janela de tempo para os lockers
- O custo e o tempo da viagem de cada aresta é igual a distância entre aqueles nós
- O custo de atribuição de um cliente no locker é igual a distância do cliente até o locker
- Todos clientes possuem demanda igual a 1
- O valor de R para o conjunto fi, é igual a 15.
- A capacidade de cada locker é igual a 5
- A capacidade do veículo é (quantidade_de_clientes/2)
- Os parâmetros da object functions é: (alfa_1 = 1) e (alfa_2 = 0.5) e (alfa_3 = 0)
- Lockers e clientes não possuem tempo de serviço