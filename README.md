# Blackjack_MSP430_Game

![ed229c0e-40c4-48e5-87d7-8f14e7bd71db](https://user-images.githubusercontent.com/55175448/128102065-81b10a36-f090-419b-ad35-92015149378b.jpg)


The blackjack game was created on a TI MSP430F5529 board. A sharp 128 x 128 LCD display was used to show the player and dealer cards, and prompt the player through the next steps. The entire game was implemented in C programming language with the help of state machines.

The game first displays a welcome page, and prompts the plyer to press a launchpad button. Then, the player is prompted to press a button wherever he/she wants to cut the deck. The game then displays the first round of both the dealer and player cards as well as their respective points and prompts the player if he/she wants to "hit" or "stay". When the player chooses to "hit", he/she is given a new card and the displayed points are adjusted accordingly. The player can choose to hit as many times as he/she desires. When the player chooses "stay", the dealer takes cards from the deck until his points are greater than or equal to 17. Finally, the scores are calcualted and the winner is announced. When the game is over, the player has a choice whether he/she wants to play the game again.

For a more detailed report please refer to: [ECE 2049 Lab 1 Report.pdf](https://github.com/Atharva4320/Blackjack_MSP430_Game/files/6927698/ECE.2049.Lab.1.Report.pdf)
