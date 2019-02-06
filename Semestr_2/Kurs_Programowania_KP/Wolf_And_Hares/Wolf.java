import java.util.List;

/**
* Klasa wilka
* @author Jan Brzezanski
* @version 1.0
*/
public class Wolf extends Thread{
	
	/**
	* Dwuwymiarowa tablica pol na planszy
	*/
	Field[][] fields;
	
	/**
	* Lista wszystkich zajacow
	*/
	List<Hare> hares;
	
	/**
	* Wartosc szerokosci pola, na ktorym znajduje sie dany wilk
	*/
	public int m;
	
	/**
	* Wartosc wysokosci pola, na ktorym znajduje sie dany wilk
	*/
	public int n;
	
	/**
	* Wartosc szerokosci pola, na ktorym znajduje sie najblizszy zajac
	*/
	public int hx;
	
	/**
	* Wartosc wysokosci pola, na ktorym znajduje sie najblizszy zajac
	*/
	public int hy;
	
	/**
	* Panel, na ktorym jest plansza
	*/
	Board b;
	
	/**
	* Wartosc opoznienia
	*/
	int k;
	
	/**
	* Zmienna mowiaca, czy wilk musi odczekac po zjedzeniu zajaca
	*/
	int wait = 0;

	/**
	* Konstruktor klasy Wolf, przypisuje parametry do zmiennych obiektu:
	* @param fields
	*		dwuwymiarowa tablica pol na planszy
	* @param m
	*		wartosc szerokosci pola, na ktorym znajduje sie dany wilk
	* @param n
	*		wartosc wysokosci pola, na ktorym znajduje sie dany wilk
	* @param k
	*		wartosc opoznienia
	* @param b
	*		panel, na ktorym jest plansza
	* @param list
	*		lista wszystkich zajacow
	*/
	Wolf(Field[][] fields, int m, int n, int k, Board b, List<Hare> list){
		this.fields = fields;
		this.m = m;
		this.n = n;
		this.k = k;
		this.b = b;
		this.hares = list;
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
	* Funkcja sprawdzajaca, czy odleglosc pola (x1,x2) od zajaca jest blizsza niz od pola, na ktorym wilk sie znajduje w tej chwili
	* @param x1
	*		wspolrzedna x punktu
	* @param y1
	*		wspolrzedna y  punktu
	* @return 
	*		true, jezeli jest blizsza, false w p.p.
	*/
	public boolean distance2(int x1,int y1){
		if (Math.pow(x1-hx,2)+Math.pow(y1-hx,2) < Math.pow(m-hx,2)+Math.pow(n-hy,2)){
			return true;		
		}
		return false;	
	}
	
	/**
	* Funkcja odpowiedzialna za ruch i myslenie wilkow
	* Ustala polozenie najblizszego zajaca;
	* Wybiera pole najblizej oddalone od tego zajaca;
	* Przechodzi na nie;
	* Jezeli jest na nim wilk, to go zjada i ustala wartosc zmiennej wait na 4 - czyli czekanie 5 cykli
	*/
	public void run(){ 
		while (true){
			wait++;
			while(wait>0){
				try{
					Thread.sleep(k);
					wait--;
				} 
				catch(InterruptedException ex){
					Thread.currentThread().interrupt();
				}
			}
			synchronized(fields){
				synchronized(hares){		
					if(hares.size()==0)
						break;
					int min = distance(m,n,hares.get(0).m,hares.get(0).n);
					
					for (int i=1; i< hares.size(); i++)
						if (distance(m,n,hares.get(i).m,hares.get(i).n) < min)
							min = distance(m,n,hares.get(i).m,hares.get(i).n);
					
					int who=0;
					
					for (int i=0; i< hares.size(); i++)
						if (distance(m,n,hares.get(i).m,hares.get(i).n) == min){
							who = i;
							break;
						}
							
					hx=hares.get(who).m;
					hy=hares.get(who).n;
					
					int moved = 0;
					int minx=m;
					int miny=n;
					int mindist = distance(m,n,hx,hy);
					
					for (int i=-1; i < 2; i++)
						for (int j=-1; j<2; j++)
							if (distance(m-i,n-j,hx,hy) < mindist && (fields[m-i][n-j].getType()==0 || fields[m-i][n-j].getType()==1)){
								mindist = distance(m-i,n-j,hx,hy);
								minx=m-i;
								miny=n-j;
							}
					
					fields[m][n].setType(0);
					m=minx;
					n=miny;

					if (fields[m][n].getType()==1){
						for (int k=0; k<hares.size(); k++)
							if (hares.get(k).m == m && hares.get(k).n==n){
								hares.get(k).kill();
								hares.remove(k);
							}
							wait=4;
							System.out.println("OMNOMNOMNOMNOMNOM");
					}
					fields[m][n].setType(2);

					b.repaint();
					yield();
				}
			}
		}
    }
}