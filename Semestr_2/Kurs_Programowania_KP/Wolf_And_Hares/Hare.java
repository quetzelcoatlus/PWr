import java.util.List;

/**
* Klasa zajaca
* @author Jan Brzezanski
* @version 1.0
*/
public class Hare extends Thread{
	
	/**
	* Dwuwymiarowa tablica pol na planszy
	*/
	Field[][] fields;
	
	/**
	* Lista wszystkich wilkow
	*/
	List<Wolf> wolfs;
	
	/**
	* Wartosc szerokosci pola, na ktorym znajduje sie dany zajac
	*/
	public int m;
	
	/**
	* Wartosc wysokosci pola, na ktorym znajduje sie dany zajac
	*/
	public int n;
	
	/**
	* Wartosc szerokosci pola, na ktorym znajduje sie najblizszy wilk
	*/
	public int wx;
	
	/**
	* Wartosc wysokosci pola, na ktorym znajduje sie najblizszy wilk
	*/
	public int wy;
	
	/**
	* Panel, na ktorym jest plansza
	*/
	Board b;
	
	/**
	* Wartosc opoznienia
	*/
	int k;
	
	/**
	* Zmienna mowiaca, czy zajac zyje, czy nie
	*/
	public boolean alive;
	
	/**
	* Konstruktor klasy Hare, przypisuje parametry do zmiennych obiektu:
	* @param fields
	*		dwuwymiarowa tablica pol na planszy
	* @param m
	*		wartosc szerokosci pola, na ktorym znajduje sie dany zajac
	* @param n
	*		wartosc wysokosci pola, na ktorym znajduje sie dany zajac
	* @param k
	*		wartosc opoznienia
	* @param b
	*		panel, na ktorym jest plansza
	* @param list
	*		lista wszystkich wilkow
	*/
	Hare(Field[][] fields, int m, int n, int k, Board b, List<Wolf> list){
		this.fields = fields;
		this.m = m;
		this.n = n;
		this.k = k;
		this.b = b;
		this.wolfs = list;
		this.alive = true;
		
	}
	
	/**
	* Funkcja liczaca kwadrat odleglosci punktu (x1,y1) od (x2,y2)
	* @param x1
	*		wspolrzedna x pierwszego punktu
	* @param y1
	*		wspolrzedna y pierwszego punktu
	* @param x2
	*		wspolrzedna x drugiego punktu
	* @param y2
	*		wspolrzedna y drugiego punktu
	* @return 
	*		wartosc kwadratu odleglosci punktow
	*/
	public int distance(int x1, int y1, int x2, int y2){
		return (int) (Math.pow(x1-x2,2)+Math.pow(y2-y1,2));
	}
	
	/**
	* Funkcja sprawdzajaca, czy odleglosc pola (x1,x2) od wilka jest dalsza niz od pola, na ktorym zajac sie znajduje w tej chwili
	* @param x1
	*		wspolrzedna x punktu
	* @param y1
	*		wspolrzedna y  punktu
	* @return 
	*		true, jezeli jest dalsza, false w p.p.
	*/
	public boolean distance2(int x1,int y1){
		if (Math.pow(x1-wx,2)+Math.pow(y1-wy,2) > Math.pow(m-wx,2)+Math.pow(n-wy,2)){
			return true;		
		}
		return false;	
	}
	
	/**
	* Funkcja zabijajaca zajaca
	*/
	public void kill(){
		alive=false;
	}
	
	/**
	* Funkcja odpowiedzialna za ruch i myslenie zajacow
	* Ustala polozenie najblizszego wilka;
	* Wybiera pole najdalej oddalone od tego wilka, jezeli takie istnieje;
	* I przechodzi na nie
	*/
	public void run(){ 
		while (alive){
			try{
				Thread.sleep(k);
			} 
			catch(InterruptedException ex){
				Thread.currentThread().interrupt();
			}
			synchronized(fields){			
				synchronized(wolfs){
					if (fields[m][n].getType() == 2)
						break;
					
					int min = distance(m,n,wolfs.get(0).m,wolfs.get(0).n);
									
					for (int i=1; i< wolfs.size(); i++)
						if (distance(m,n,wolfs.get(i).m,wolfs.get(i).n) < min)
							min = distance(m,n,wolfs.get(i).m,wolfs.get(i).n);
					
					int who=0;
					
					for (int i=0; i< wolfs.size(); i++)
						if (distance(m,n,wolfs.get(i).m,wolfs.get(i).n) == min){
							who = i;
							break;
						}
							
					wx=wolfs.get(who).m;
					wy=wolfs.get(who).n;
		
					int maxx = m;
					int maxy = n;
					int maxdist = distance(m,n,wx,wy);
					
					for (int i=-1; i < 2; i++)
						for (int j=-1; j<2; j++)
							if (distance(m-i,n-j,wx,wy) > maxdist && fields[m-i][n-j].getType()==0){
								maxdist = distance(m-i,n-j,wx,wy);
								maxx=m-i;
								maxy=n-j;
							}
					
					if(maxx != m || maxy != n){
						fields[m][n].setType(0);
						m = maxx;
						n = maxy;
						fields[m][n].setType(1);
					}
					
					b.repaint();
					yield();
				}
			}
		}
    }
}