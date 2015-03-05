//===--- bresenham.h - C++ Implementation of 4D Bresenham Algo --*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __bresenham_h
#define __bresenham_h

#include <glm/glm.hpp>

static inline int32_t pbSignum(float x) {
  return x < 0 ? -1 : 1;
}

// The algorithm for 4D bresenham line drawing was taken from the following and ported to C++
// https://sites.google.com/site/proyectosroboticos/bresenham-6d
void bresenham4D(glm::vec4 start, glm::vec4 dir, glm::ivec4 *result, size_t resultSize) {
  glm::vec4 here = start;

  int32_t xInc = pbSignum(dir.x);
  int32_t yInc = pbSignum(dir.y);
  int32_t zInc = pbSignum(dir.z);


}

/*
Dim As Integer Cont, dx, dy, dz, dw, Adx, Ady, Adz, Adw, x_inc, y_inc, z_inc, w_inc,_
               err_1, err_2, err_3, dx2, dy2, dz2, dw2, xxx, yyy, zzz, www,_
               Xold, Yold, Zold, Wold, Xnew, Ynew, Znew, Wnew

Dim As String  Q1, Q2, Q3, Q4

While 1

   Color 1, 0

	Print "Para -Salir del programa- pulse 'Enter' sin introducir ningun valor."

	Color 9,0

   Input "Introduce 4 coordenadas separados por comas. x,y,z,w : ", Q1, Q2, Q3, Q4

	If Q1 = "" Then End

	Color 14,0

	Xnew = Val(Q1)
	Ynew = Val(Q2)
	Znew = Val(Q3)
	Wnew = Val(Q4)

	Print xxx, yyy, zzz, www

	dx = Xnew - Xold
	dy = Ynew - Yold
	dz = Znew - Zold
	dw=  Wnew - Wold

	If (dx < 0) Then
		 x_inc = -1
	Else
		 x_inc =  1
	EndIf

	If (dy < 0) Then
	  y_inc = -1
	Else
	  y_inc =  1
	EndIf

	If (dz < 0) Then
		z_inc = -1
	Else
		z_inc =  1
	EndIf

	If (dw < 0) Then
		w_inc = -1
	Else
		w_inc =  1
	EndIf

	Adx = Abs(dx)
	Ady = Abs(dy)
	Adz = Abs(dz)
	Adw = Abs(dw)

	dx2 = Adx*2
	dy2 = Ady*2
	dz2 = Adz*2
	dw2 = Adw*2

	If (Adx>= Ady) And (Adx>= Adz) And (Adx>= Adw) Then

		err_1 = dy2 - Adx
		err_2 = dz2 - Adx
		err_3 = dw2 - Adx

		For Cont = 1 To Adx

		   If (err_1 > 0) Then
		       yyy+= y_inc
		       err_1 -= dx2
		   EndIf

		   If (err_2 > 0) Then
		       zzz+= z_inc
		       err_2 -= dx2
		   EndIf

		   If (err_3 > 0) Then
		       www+= w_inc
		       err_3 -= dx2
		   EndIf

		   err_1 += dy2
		   err_2 += dz2
		   err_3 += dw2

		   xxx+= x_inc

		   Print xxx, yyy, zzz, www

		Next

	EndIf

	If (Ady> Adx) And (Ady>= Adz) And (Ady>= Adw) Then

		err_1 = dx2 - Ady
		err_2 = dz2 - Ady
		err_3 = dw2 - Ady

		For Cont = 1 To  Ady

		   If (err_1 > 0) Then
		       xxx+= x_inc
		       err_1 -= dy2
		   EndIf

		   If (err_2 > 0) Then
		       zzz+= z_inc
		       err_2 -= dy2
		   EndIf

			If (err_3 > 0) Then
				 www+= w_inc
				 err_3 -= dy2
			EndIf

		   err_1 += dx2
		   err_2 += dz2
		   err_3 += dw2

		   yyy+= y_inc

			Print xxx, yyy, zzz, www

		Next

	EndIf

	If (Adz> Adx) And (Adz> Ady) And (Adz>=Adw) Then

		err_1 = dy2 - Adz
		err_2 = dx2 - Adz
		err_3 = dw2 - AdZ

		For Cont = 1 To Adz

		   If (err_1 > 0) Then
		       yyy+= y_inc
		       err_1 -= dz2
		   EndIf

		   If (err_2 > 0) Then
		       xxx+= x_inc
		       err_2 -= dz2
		   EndIf

		   If (err_3 > 0) Then
		       www+= w_inc
		       err_3 -= dz2
		   EndIf

		   err_1 += dy2
		   err_2 += dx2
		   err_3 += dw2

		   zzz+= z_inc

			Print xxx, yyy, zzz, www

		Next

	EndIf

	If (Adw> Adx) And (Adw> Ady) And (Adw> Adz) Then

		err_1 = dy2 - Adw
		err_2 = dx2 - Adw
		err_3 = dz2 - Adw

		For Cont = 1 To Adw

		   If (err_1 > 0) Then
		       yyy+= y_inc
		       err_1 -= dw2
		   EndIf

		   If (err_2 > 0) Then
		       xxx+= x_inc
		       err_2 -= dw2
		   EndIf

		   If (err_3 > 0) Then
		       zzz+= z_inc
		       err_3 -= dw2
		   EndIf

		   err_1 += dy2
		   err_2 += dx2
		   err_3 += dz2

		   www+= w_inc

			Print xxx, yyy, zzz, www

		Next

	EndIf

	Xold=Xnew
	Yold=Ynew
	Zold=Znew
	Wold=Wnew

Wend

End
*/

#endif // defined(__bresenham_h)
