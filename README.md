<h2>Projeto de Sistema em Tempo Real: Barbeiros dorminhocos </h2>

<h3>Descrição</h3>
<h4>Resumo</h4> 
<span>Projeto baseado no clássico problema do Barbeiro Dorminho proposto por Edsger Dijkstra - que envolve sincronização em sistemas operacionais e programação concorrente. O problema foi proposto para ilustrar o uso de semáforos na resolução de problemas de sincronização. Este projeto aborda <b>problemas de exclusão mútua, deadlock, starvation e sincronização.</b> A solução contida neste neste repositório permite a troca de informação entre o cliente e o barbeiro, uma simples conversa entre ambos envolve a troca dos identificados (ids) assim como a sinalização de alguns estados como dormindo/acordado.
</span>

<h4>Cenário</h4>
<ol>
    <li>Salão de barbeiros: O salão possui 3 barbeiros e 5 cadeiras de espera.</li>
    <li>Barbeiros: dormem quando não há clientes e acordam quando um cliente chega para ser atendido.</li>
    <li>Clientes: chegam aleatoriamente no salão. Se todas as cadeiras de espera estiverem ocupadas, o cliente vai embora sem cortar o cabelo.</li>
    <li>Cadeiras de espera: capacidade para 5 clientes. Clientes aguardam em uma dessas cadeiras se os barbeiros estiverem ocupados.</li>
</ol>

A seguir uma breve descrição de como os <b>problemas de exclusão mútua, deadlock, starvation e sincronização.</b> foram abordados.

<h4>Starvation</h4>
<span>O problema de Starvation ocorre quando uma ou mais tarefas em um sistema não conseguem obter os recursos necessários para progredi, resultando em atrasos indefinidos ou a incapacidade de completar suas operações.</span>

<br>

<span> Para evitar Starvation, foi utilizado uma fila lógica FIFO (First-in-First-out) para priorizar o processamento dos clientes que chegam primeiro, quando um cliente termina, o próximo na fila é chamado. Assim, nenhum cliente espera eternamente ou é injustiçado no decorrer do programa. </span>

<h4>Sincronização</h4>
<span>O problema de sincronização ocorre em sistemas de computação concorrentes, onde múltiplos processos ou threads precisam acessar e manipular recursos compartilhados, como memória, arquivos ou dispositivos de hardware. Se esses acessos não forem devidamente coordenados, pode haver conflitos que resultam em comportamentos indesejados, como condições de corrida, inconsistências de dados, deadlocks e starvation.</span>

<br>

<span> Em alguns pontos é necessário que tanto o thread do cliente quanto a thread do barbeiro estejam sincronizadas, isto ocorre em alguns momentos para notificar as thread que determinada ação pode ser tomada. Sendo elas:</span>
<ul>
    <li> Avisar ao barbeiro que há clientes (quando mesmo está dormindo/suspenso).
    <li> Avisar ao cliente que o barbeiro acordou (parte da politica para troca de informações).
    <li> Notificar ao cliente que o corte terminou e que ele pode seguir.
    <li> Chamar algum processo cliente suspenso que esteja aguardando na fila.
</ul>

<b>Diagrama funcional:</b>
![Diagrama](img/Diagram.png)
 
[Link do video no Youtube](https://youtu.be/qrJ4FP_Sh9c) 

<br>

<h3> Real Time Systems: Sleeping Baber </h3>
<span><b>Description:</b> A problem based on the classic Sleeping Barber problem proposed by Edsger Dijkstra - which involves synchronization in operating systems and concurrent programming. The problem was proposed to illustrate the use of semaphores in solving synchronization problems.</span>

<h4>Dependencies</h4>
<ul>
    <li> make
    <li> pthread
</ul>


