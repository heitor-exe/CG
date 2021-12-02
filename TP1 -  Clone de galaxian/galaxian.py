from pplay.window import *
from pplay.sprite import *
from pplay.gameimage import *
from pplay.sound import *
from pplay.keyboard import *

import keyboard
import random
random.seed()

# Controla o estágio do jogo:
# 0 - Menu inicial
# 1 - Jogo
# 2 - Pontuação final
GAME_SPEED = 1
GAME_STATE = 0

# Largura da janela
width = 400

# Altura da janela
height = 600

# Cor do fundo
background_color = [0, 0, 0]

# Titulo da janela
title = "Galaxian"

window = Window(width, height)
window.set_title(title)
window.set_background_color(background_color)

# Imagem do fundo - 2 para scrolling
background_01 = GameImage("stars_background.png")
background_02 = GameImage("stars_background.png")

# Posições iniciais
background_01.y = 0
background_02.y = -background_02.height

# Velocidade de rolagem
background_roll_speed = 100

# Controles
keyboard = window.get_keyboard()
mouse = window.get_mouse()

# Som de tiro
bullet_sound = Sound("bullet_sound.ogg")

# Sprite nave
player = Sprite("ship.png")

# Posição inicial
player.set_position((window.width - player.width)/2, (window.height - player.height))

# Velocidade da nave
player.speed = 200

# Direção do jogador
player.direction = -1

# Pontuação 
player.score = 0

# Sprite inimigos
enemy_image = "red_enemy.png"

# Velocidade inimigos
enemy_speed = 200

# Direção inimigos
enemy_direction = 1

# Matriz de inimigos
# Tamanho limite matriz
MAXSIZE = 10

# Numero linhas
matrix_x = int(random.uniform(3, MAXSIZE))

# Numero colunas
matrix_y = int(random.uniform(1, MAXSIZE))

# Vetor de tiros
bullets = []

# Vetor inimigos
enemies = [[0 for x in range(10)] for x in range(10)]

# Controladores temporais
enemy_shoot_delay = 1/GAME_SPEED
player.shoot_delay = 1/GAME_SPEED * 0.5

player.shoot_tick = player.shoot_delay

# Função que verifica se o jogador ganhou
def win():

    # Acesso às variáveis globais
    global matrix_x
    global matrix_y
    global GAME_STATE

    # Variável de controle
    won = True

    # Verificação em todas as linhas se ainda há um inimigo
    for row in range(matrix_x):
        if won:
            for column in range(matrix_y):
                if enemies != 0:
                    # Se um inimigo for encontrado,
                    # won torna-se falso e a cadeia
                    # de repetições é quebrada
                    won = False
                    break 
    
    if won:
        # Se toda a matriz for percorrida
        # e nenhum inimigo foi encontrado,
        # o jogo reinicia
        GAME_STATE = 2

# Função para reiniciar o jogo
def restart(player, enemies, bullets):

    # Acesso às variáveis globais
    global matrix_x
    global matrix_y

    # Deleta todos os inimigos e bullets
    del enemies
    del bullets

    # Retorna o jogador à posição e pontuação inicial
    player.score = 0
    player.set_position((window.width - player.width)/2, (window.height - player.height))

    # Reinicia os contadores de tiro
    player.shoot_tick = player.shoot_delay

    # Nova matriz de inimigos
    matrix_x = int(random.uniform(3, 10))
    matrix_y = int(random.uniform(1, 10))
    spawn_enemy(matrix_x, matrix_y, enemies)

# Função para ajustar posição do atirador e da bala
def adjust_bullet(actor, bullet):

    # Cálculo da posição X da bala
    x_fire = actor.x + (actor.width / 2) - (bullet.width / 2)

    # Cálculo da posição Y da bala, usando
    # a direção de movimento e o tamanho do
    # jogador como referência
    if actor.direction == -1:
        y_fire = actor.y

    elif actor.direction == 1:
        y_fire = actor.y + actor.height - bullet.height
    
    # Transferência do valor das variáveis auxiliares para a bala
    bullet.x = x_fire
    bullet.y = y_fire

    # Direção da bala
    bullet.direction = actor.direction

# Função para criar a bala associando-a a um ator
def shoot(shooter):

    # Som do tiro
    bullet_sound.play()

    # Contador do último tiro
    shooter.shoot_tick = 0

    # Criação de nova bala
    if shooter.direction == -1:
        b = Sprite("bullet_player.png")
    
    elif shooter.direction == 1:
        b = Sprite("bullet_enemy.png")

    # Ajuste da posição inicial e a direção da bala
    adjust_bullet(shooter, b)

    # Adiciona a nova bala para ser desenhada na tela
    bullets.append(b)

# Função para gerar a matriz de inimigos
def spawn_enemy(i, j, enemy_matrix):

    for x in range(i):
        for y in range(j):
            # Sprite inimigo
            enemy = Sprite(enemy_image)

            # Posição
            enemy.set_position(x * enemy.width, y * enemy.height)

            # Direção do movimento
            enemy.direction = 1

            # Intervalo aleatório entre os disparos
            enemy.shoot_delay = random.uniform(0, 15) / GAME_SPEED

            # Zera variável do controle dos tiros
            enemy.shoot_tick = 0

            # Adiciona o inimigo na matriz
            enemy_matrix[x][y] = enemy

def scrolling(bg_bottom, bg_top, roll_speed):

    # Movimentação dos sprites verticalmente
    bg_bottom.y += roll_speed * window.delta_time()
    bg_top.y += roll_speed * window.delta_time()

    # Se a imagem de cima já tiver sido exibida,
    # retorna ambas imagens para a as posições iniciais
    if bg_top.y >= 0:
        bg_bottom.y = 0
        bg_top.y = -bg_top.height

    # Renderização das imagens de fundo
    bg_bottom.draw()
    bg_top.draw()

# Função para atualizar os contadores do jogo
def update_counters():

    # Contador de controle tiro do jogador
    player.shoot_tick += window.delta_time()

    # Contador de controle de tiro de cada inimigo
    for row in range(matrix_x):
        for column in range(matrix_y):
            if enemies != 0:
                enemies.shoot_tick += window.delta_time()

# Função para cirar os disparos do jogador
def player_shoot():

    # Verifica se o jogador apertou o espaço
    if keyboard.key_pressed("space"):
        # Verifica se o tiro pode ser disparado
        if player.shoot_tick > player.shoot_delay:
            shoot(player)

# Função para controlar o movimento do jogador
def player_movement():

    # Atualiza posição do jogador
    player.move_key_x(player.speed * window.delta_time() * GAME_SPEED)

    # Impede que a nave ultrapasse a tela
    if player.x <=0:    
        player.x = window.width - player.width

# Função que faz a movimentação de cada bala
def bullet_movement():

    for b in bullets:

        # Atualiza a posição, de acordo com a direção
        b.move_y(200 * b.direction * window.delta_time() * GAME_SPEED)

        # Verificação se a bala saiu da tela, para removê-la
        if b.y < -b.height or b.y > window.height + b.height:
            bullets.remove(b)

# Função que faz a movimentação dos inimigos
def enemy_movement():

    # Acesso das variáveis globais
    global enemy_direction
    global enemy_speed

    # Variável de controle
    inverted = False

    # Cálculo da nova posição da matriz de inimigos
    new_position = enemy_speed * enemy_direction * window.delta_time() * GAME_SPEED

    # for para percorrer a matriz de inimigos
    for row in range(matrix_x):
        for column in range(matrix_y):

            # Se a posição já estiver preenchida (inimigo vivo),
            # efetue as ações em seguida
            if enemies !=0:
                # Move inimigo para nova posição
                enemies.move_x(new_position)

                # Se o intervalo de tiro foi alcançado,
                # realize um novo tiro
                if enemies.shoot_delay:
                    shoot(enemies)
                    enemies.shoot_tick = 0

                    # Novo intervalo de tiro aleatório
                    enemies.shoot_delay = random.uniform(0,15) / GAME_SPEED

                # Verificação de encontro da matriz com a parede
                if not inverted:
                    # Se encontrou com a parede,
                    # então inverta a direção da matriz
                    if enemies.x <= 0:                        
                        enemy_direction = 1                        
                        inverted = True                     
                    # Altera direção para esquerda 
                    elif enemies.x + Sprite(enemy_image).width >= window.width:
                        enemy_direction = -1
                        inverted = True

# Função que verifica a colisão dos disparos
def bullet_ship_collision():
    global GAME_STATE

    for b in bullets:
        # Se for disparo do jogador
        if b.direction == -1:
            check_enemy_collision(b)

        #Se for disparo do inimigo
        elif b.direction == 1:
            if b.collided(player):
                GAME_STATE = 2

# Função que verifica se algum tiro acertou um inimigo
def check_enemy_collision(b):

    # for para percorrer a matriz de inimigos
    for row in range(matrix_x):
        for column in range(matrix_y):

            # Se o inimigo estiver vivo (enemies!= 0),
            # verifique se o tiro b acertou o inimigo
            if enemies!= 0:

                # Se ocorreu colisão,
                # remova a bala e o inmigo
                if b.collided(enemies):
                    bullets.remove(b)
                    enemies = 0

                    # Adiciona pontos ao jogador
                    player.score += 50

                    # Interrupção da função
                    return

# Função para verificar se um tiro colidiu com outro tiro
def bullet_bullet_collision():

    for b1 in bullets:
        # Se for tiro do jogador
        if b1.direction == -1:
            # Verifica se colidiu com outro
            for b2 in bullets:
                # Verifica se o tiro é do inimigo
                if b2.direction == 1:
                    # Se for inimigo, ocorreu colisão
                    if b1.collided(b2):
                        # Se ocorreu colisão, remova os dois tiros
                        bullets.remove(b1)
                        bullets.remove(b2)

                        break

# Função que desenhará todos os elementos do jogo
def draw():

    # Desenha os tiros
    for b in bullets:
        b.draw()

    # for para percorrer a matriz de inimigos
    for row in range(matrix_x):
        for column in range(matrix_y):

            # Se o inimigo está vivo (!=0), desenhe o inimigo
            if enemies != 0:
                enemies.draw()

    # Desenha nave
    player.draw()

# Função para criar a janela do jogo
def start_window():

    global GAME_STATE

    window.draw_text("ENTER -> jogar | ESC -> sair", 0, 0, 28, (255, 255, 255), "Calibri", True)

    # Se 'enter' foi pressionado, inicie o jogo
    if (keyboard.key_pressed("ENTER")):
        GAME_STATE = 1
        restart(player, enemies, bullets)

    # Se 'esc' foi pressionado, feche o jogo
    elif (keyboard.key_pressed("ESC")):
        window.close()

# Função que reinicia o jogo
def restart_window():

    global GAME_STATE

    window.draw_text("SUA PONTUAÇÃO: " + str(player.score), 5, 5, 16, (255, 255, 255), "Calibri", True)

    # Se o jogador pressiona 'enter', reinicie o jogo
    if keyboard.key_pressed("ENTER"):
        GAME_STATE = 1
        restart(player, enemies, bullets)

# Função que roda o jogo infinitamente, até ser fechado
while True:

    # Escurece tela
    window.set_background_color(background_color)

    # Função que rola o fundo
    scrolling(background_01, background_02, background_roll_speed)

    # Se for 0, é a primeira vez sendo executado
    if GAME_STATE == 0:
        start_window()

    # Se for 1, o jogo ainda está rodando
    elif GAME_STATE == 1:

        # Verificação se o jogador venceu
        win()

       # Atualiza os contadores
        update_counters()
 
        # Atualiza a movimentação do jogador
        player_movement()
 
        # Controle os tiros a cada intervalo
        player_shoot()
 
        # Atualiza o movimento dos disparos
        bullet_movement()
 
        # Atualiza o movimento dos inimigos
        enemy_movement()
 
        # Verifica a colisão de projéteis contra naves
        bullet_ship_collision()
 
        # Verifica colisões entre projéteis
        bullet_bullet_collision()
 
        # Desenha os dados na tela ##
        draw()

    # Se o jogo terminou, reinicie a partida
    elif GAME_STATE == 2:
        restart_window()
    window.update()
