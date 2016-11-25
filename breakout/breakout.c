//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

typedef struct Collision{
    GObject *object;
    int type;
} Collision;

// prototypes
void removeOp(GObject gobj);
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
Collision detectCollision(GWindow window, GOval ball);
void centerLabel(GWindow window, GLabel label);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    int diameter = 2 * RADIUS;
    int xVel = 1;
    int yVel = 2;

    int pauseTime = 10;
    int pauseDec = 3;

    GObject object = NULL;

    char *score[4];
    // keep playing until game over
    while (lives > 0 && bricks > 0) {
        move(ball, xVel, yVel);
        
        if (getX(ball) + diameter >= WIDTH || getX(ball) <= 0) {
            xVel = -xVel;
        }

        if (getY(ball) <= 0) {
            yVel = -yVel;
        }

        if (getY(ball) + diameter >= HEIGHT) {
            yVel = -yVel;

            // lives --;
            // pauseTime = 10;
        }

        Collision collision = detectCollision(window, ball);

        if (collision.object != NULL) {
            if (paddle == collision.object) {
                // bricks = 0;
            } else if (strcmp(getType(collision.object), "GRect") == 0) {
                setLocation(collision.object, -100, -100); 
                removeOp(collision.object);

                bricks --;
                if (collision.type == TOP || collision.type == BOTTOM) {
                    yVel = -yVel;
                } 
                if (collision.type == LEFT || collision.type == RIGHT) {
                    xVel = -xVel;
                }
                
                points += (abs(pauseTime - 10) + 1) * 1;
                updateScoreboard(window, label, points);

                pauseDec--;
                if (pauseDec <= 0) {
                    pauseDec = 3;
                    if (pauseTime > 3) {
                        pauseTime--;
                    }
                }
            }
        }

        pause(pauseTime);
    }

    if (lives == 0) {
        setColor(label, "0x990000");
        setLabel(label, "You Loose!");
        centerLabel(window, label);
    }
    
    if (bricks == 0) {
        char msg[40];
        sprintf(msg, "You Won this game. Your score is: %i", points);
        setColor(label, "0x009900");
        setLabel(label, msg);
        centerLabel(window, label);
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    char *color[8];
    int x, y,
    rc = 22,
    gc = 55,
    bc = 88, 
    w = (WIDTH - 20) / COLS - 5, 
    h = 10;
    for (int r = 0; r < ROWS; r++) {
        y = 50 + 15 * r;
        rc = (int)pow(rc, (r+1.0)) % 255;
        gc = (int)pow(gc, (r+1.0)) % 255;
        bc = (int)pow(bc, (r+1.0)) % 255;
        sprintf(*color, "0x%x%x%x", (rc & 0xff), (gc & 0xff), (bc & 0xff));

        for (int c = 0; c < COLS; c++) {
            x = 10 + (w + 5) * c;
            GRect brick = newGRect(x, y, w, h);
            setColor(brick, *color);
            setFilled(brick, true);
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS, RADIUS * 2,  RADIUS * 2);
    setColor(ball, "gray");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(WIDTH / 2 - RADIUS * 3, HEIGHT - 20, 6 * RADIUS, 5);
    setColor(paddle, "darkgray");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "Dialog-16");
    setColor(label, "0xAAAAAA");
    add(window, label);
    centerLabel(window, label);

    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    centerLabel(window, label);
}

void centerLabel(GWindow window, GLabel label)
{
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
Collision detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;
    Collision collision;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x + RADIUS, y-1);
    if (object != NULL)
    {
        collision.type = TOP;
    } 
    
    if (object == NULL) {
        // check for collision at ball's top-right corner
        object = getGObjectAt(window, x + RADIUS, y + 1 + 2 * RADIUS);
        if (object != NULL)
        {
            collision.type = BOTTOM;
        }
    }

    if (object == NULL) {
        // check for collision at ball's bottom-left corner
        object = getGObjectAt(window, x-1, y + RADIUS);
        if (object != NULL)
        {
            collision.type = LEFT;
        }
    }

    if (object == NULL) {
        // check for collision at ball's bottom-right corner
        object = getGObjectAt(window, x + 1 + 2 * RADIUS, y + RADIUS);
        if (object != NULL)
        {
            collision.type = RIGHT;
        }
    }

    if (object != NULL) {
        printf("touch %s \n", getType(object));
    }
    collision.object = object;
    
    return collision;
}
