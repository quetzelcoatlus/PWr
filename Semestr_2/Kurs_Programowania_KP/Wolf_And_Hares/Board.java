import javax.swing.JPanel;
import java.awt.Graphics;
import java.util.List;
import java.util.ArrayList;
import java.util.Random;
import java.awt.Color;
import javax.swing.JOptionPane;


/**
* Klasa panelu, na ktorym jest plansza
* dziedziczy po JPanel
* @author Jan Brzezanski
* @version 1.0
*/
public class Board extends JPanel{
	
	/**
	* Dwuwymiarowa tablica pol na planszy
	*/
	Field[][] fields;
	
	/**
	* Lista wszystkich zajacow
	*/
	public List<Hare> hares;
	
	/**
	* Lista wszystkich wilkow
	*/
	public List<Wolf> wolfs;
	
	/**
	* Wartosc szerokosci planszy
	*/
	int max_m;
	
	/**
	* Wartosc wysokosci planszy
	*/
	int max_n;
	
	/**
	* Czy plansza zostala zainicjowana
	*/
	boolean initialized = false;
	
	/**
	* Glowna funkcja rysujaca;
	* Rysuje wszystkie pola planszy
	* @param g
	*		parametr graficzny używany do rysowania pol planszy
	*/
	private void doDrawing(Graphics g){
		if (initialized)
			for (int i=1; i<max_m+1; i++)
				for (int j=1; j<max_n+1; j++)
					fields[i][j].draw(g, max_m, max_n);
	}
	
	/**
	* Funkcja inicjalizujaca plansze
	* Tworzy plansze, zajace i wilki i uruchamia ich watki
	* @param max_m
	*		wartosc szerokosci planszy
	* @param max_n
	*		wartosc wysokosci planszy
	* @param z
	*		liczba zajacow
	* @param k
	*		wartosc opoznienia
	* @param w
	*		liczba wilkow
	*/
	public void init(int max_m, int max_n, int z, int k, int w){
		this.fields = new Field[max_m+2][max_n+2];
		this.max_m = max_m;
		this.max_n = max_n;
		Random r = new Random();
		hares = new ArrayList<Hare>();
		wolfs = new ArrayList<Wolf>();
			
		for (int i=0; i < max_m+2; i++)
			for (int j=0; j < max_n+2; j++){
				fields[i][j] = new Field(i,j,this);
				if (i==0 || j==0 || i==max_m+1 || j==max_n+1)
					fields[i][j].setType(-1);
				else
					fields[i][j].setType(0);
			}
			
		for (int i=1; i<=w; i++){
			int los_m;
			int los_n;
			do{
			los_m = r.nextInt(max_m)+1;
			los_n = r.nextInt(max_n)+1;
			} while (fields[los_m][los_n].getType() != 0);
			
			wolfs.add(new Wolf(fields, los_m, los_n, (int) ((r.nextDouble()+0.5)*k), this, hares));
			fields[los_m][los_n].setType(2);
		}
				
		for (int i=1; i<=z; i++){
			int los_m;
			int los_n;
			do{
			los_m = r.nextInt(max_m)+1;
			los_n = r.nextInt(max_n)+1;
			} while (fields[los_m][los_n].getType() != 0);
			
			hares.add(new Hare(fields, los_m, los_n,(int) ((r.nextDouble()+0.5)*k), this, wolfs));
			fields[los_m][los_n].setType(1);
		}
		
		for(int i = 0; i < hares.size(); i++){	
			hares.get(i).start();
		}
		for(int i = 0; i < wolfs.size(); i++){	
			wolfs.get(i).start();
		}
		initialized = true;
	}
	
	/**
	* Nadpisanie domyślnej funkcji paintComponent;
	* Wywoluje funkcje doDrawing();
	* @param g
	*		parametr graficzny
	*/
	@Override
    public void paintComponent(Graphics g){
        super.paintComponent(g);
        doDrawing(g);
    }		
}