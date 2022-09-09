# Trabalho 2 de Redes I. 2022

- Link para o vídeo: _adicionar depois_


### Roteiro
- O que é decodificação Reed-Solomon?
    - Reed-Solomon é um grupo de códigos de correção de bits.
- Quem criou?
    - Irvin Stoy Reed e Gustave Solomon. Dois engenheiros e matemáticos que contribuíram em diversas áreas da comunicação, engenharia elétrica e computação. Ambos ganharam prêmios da IEEE em razão da criação do grupo de códigos de correção de erro de bits que recebe a junção de seus sobrenomes.
- Quando foi criada?
    - Na década de 60. 
- Onde foi criado e onde é usada?
    - EUA. Originalmente foi um estudo matemático envolvendo códigos polinomiais sobre alguns campos finitos -- uma área da estudo da matemática. Mais tarde foi implementado nas missões de espaço profundo da _voyager_; foi, contudo, usado e ainda é usado em mídias físicas de conteúdo digital (como DVDs e CDS), códigos QR e transmissões como DSL.
- Como é usada? (simplificada)
    - Quando uma mensagem está com erro em 1 bit ou mais dizemos que esses erros são as "síndromes" da mensagem, pois é um conjunto de sintomas de uma mensagem com algum problema. Para realizar a decodificação Reed-Solomon primeiro calculam-se as síndromes da mensagem como raízes do polinômio gerador e do polinômio recebido. Se a síndrome for diferente de 0, significa que existe erro. O Reed-Solomon ainda é capaz de corrigir uma parcela de erros detectados. Para tal, é preciso obter a localização e a magnitude dos erros, utilizando as síndromes e os padrões de erros. Para se resolver esse conjunto de equações não lineares, são necessários os algoritmos de Reed-Solomon, que envolvem inúmeras operações matemáticas que, na prática, precisariam de um super-computador para serem resolvidas. Mas então, como conseguimos usar tais algoritmos tão rápido no nosso dia a dia em CDs e DVDs? A resposta está no hardware: foram criados hardwares de uso específico que decodificam Reed-Solomon super rápido e permitem seu uso cotidiano.


# Link do vídeo
 - https://youtu.be/JGVd-Rx3kUw
 
# Link do canva
- https://www.canva.com/design/DAFLTPJ6O8w/K-OyhmEMpt74y4uENSYwEw/edit?utm_content=DAFLTPJ6O8w&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton
