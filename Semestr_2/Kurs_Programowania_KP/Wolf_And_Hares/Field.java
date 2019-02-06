import java.awt.Color;
import java.awt.Graphics;

/**
* Klasa pola na planszy
* @author Jan Brzezanski
* @version 1.0
*/
public class Field{
	
	/**
	* Szerokosc, na ktorej znajduje sie dane pole
	*/
	int m;
	
	/**
	* Wysokosc, na ktorej znajduje sie dane pole
	*/
	int n;
	
	/**
	* Panel do rysowania na - na nim jest plansza
	*/
	Board b;
	
	/**
	* Kolor pola
	*/
	Color c = Color.gray;
	
	/**
	* Typ pola:
	* -1 - poza plansza
	* 0 - wolne pole
	* 1 - pole zajete przez zajaca
	* 2 - pole zajete przez wilka
	*/
	int type = -1;
	
	/**
	* Konstruktor klasy Field, przypisuje parametry do zmiennych obiektu:
	* @param m
	*		szerokosc, na ktorej znajduje sie pole
	* @param n
	*		wysokosc, na ktorej znajduje sie pole
	* @param b
	*		panel, na ktorym jest plansza
	*/
	Field(int m, int n, Board b){
		this.m = m;
		this.n = n;
		this.b = b;
	}
	
	/**
	* Funkcja rysujaca pole - rysuje prostokat o danym kolorze na danych wpolrzednych
	* @param g
	*		parametr graficzny używany do rysowania
	* @param max_m
	*		ilosc pol w poziomie
	* @param max_n
	*		ilosc pol w pionie
	*/
	public void draw(Graphics g, int max_m, int max_n){
		int w = b.getWidth()/max_m;
		int h = b.getHeight()/max_n;
		
		g.setColor(c);
		g.fillRect((m-1)*w+1,(n-1)*h+1,w-1,h-1);
		//g.setColor(Color.green);
		//g.drawString(Integer.toString(type),(m-1)*w+10,(n-1)*h+10);
	}
	
	/**
	* Funkcja ustawiajaca typ pola i odpowiadajacy mu kolor
	* @param t
	*		typ pola do ustalenia
	*/
	public void setType(int t){
		this.type = t;
		switch (t){
			case 0: this.c = Color.gray; break;
			case 1: this.c = Color.orange; break;
			case 2: this.c = Color.black; break;
			default: this.c = Color.gray; break;
		}
	}
	
	/**
	* Funkcja zwracajaca typ pola
	* @return
	*		Typ pola:
	* 		-1 - poza plansza
	* 		0 - wolne pole
	* 		1 - pole zajete przez zajaca
	* 		2 - pole zajete przez wilka
	*/
	public int getType(){
		return type;
	}
}