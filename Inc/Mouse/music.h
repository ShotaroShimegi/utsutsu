/*
 * music.h
 *
 *  Created on: 2019/10/29
 *      Author: 標 祥太郎
 */

//---Melody Frequency---
#define c5 523
#define c5h 554
#define d5 587
#define d5h 554
#define e5 659

#define f5 698
#define f5h 740
#define g5 783
#define g5h 830
#define a5 880
#define a5h 932
#define b5 988

#define c6 1046
#define c6h 1108
#define d6 1174
#define d6h 1244
#define e6 1318

#define f6 1397
#define f6h 1480
#define g6 1568
#define g6h 1661
#define a6 1760
#define a6h 1865
#define b6 1976

#define c7 2093
#define c7h 2217
#define d7 2349
#define d7h 2489
#define e7 2637

#define f7 2794
#define f7h 2960
#define g7 3136
#define g7h 3222
#define a7 3520
#define a7h 3729
#define b7 3951

#ifndef INC_MOUSE_MUSIC_H_
#define INC_MOUSE_MUSIC_H_

void MelodySummer(void);
void MelodyMrLawrence(void);
void MelodyRayearth(void);
void MelodyUrara(void);
void MelodyGoal(void);
void MelodyNatsumatsuri(void);
void MelodyKurenai(void);
void MelodyYamato(void);

void Melody(uint32_t,uint32_t);




#endif /* INC_MOUSE_MUSIC_H_ */
