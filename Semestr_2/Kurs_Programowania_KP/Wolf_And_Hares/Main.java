import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JTextField;
import java.awt.BorderLayout;
import java.awt.GridLayout;
import javax.swing.BorderFactory;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JOptionPane;


/**
* Glowna klasa programu
* @author Jan Brzezanski
* @version 1.0
*/
public class Main{
	
	/**
	* Pole do wpisania szerokosci planszy
	*/
	JTextField m;
	
	/**
	* Pole do wpisania wysokosci planszy
	*/
	JTextField n;
	
	/**
	* Pole do wpisania liczby zajacow
	*/
	JTextField z;
	
	/**
	* Pole do wpisania wielkosci opoznienia cyklu
	*/
	JTextField k;
	
	/**
	* Pole do wpisania liczby wilkow
	*/
	JTextField w;
	
	/**
	* Panel, na ktorym jest plansza
	*/
	Board b;
	
	/**
	* Funkcja uruchamiana podczas startu programu
	* Uruchamia funkcje init()
	* @param args
	*		paramtery programu
	*/
	public static void main(String[] args){
		Main m = new Main();
		m.init();
	}
	
	/**
	* Funkcja inicjalizujaca
	* Inicjalizuje okno i naklada na nie wszysktie komponenty
	*/
	public void init(){
		JFrame f = new JFrame("Wilki i zajace");
		f.setSize(1216,667);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new BorderLayout());
		f.setLocationRelativeTo(null);        
		
		JPanel p = new JPanel();
		p.setLayout(new GridLayout(1,13));
		p.setBorder(BorderFactory.createLineBorder(Color.black));
		
		JLabel mLabel = new JLabel("Szerokosc =", JLabel.RIGHT);
		JLabel nLabel = new JLabel("Wysokosc =", JLabel.RIGHT);
		JLabel zLabel = new JLabel("Zajace =", JLabel.RIGHT);
		JLabel kLabel = new JLabel("Opoznienie =", JLabel.RIGHT);
		JLabel wLabel = new JLabel("Wilki =", JLabel.RIGHT);
		
		m = new JTextField();
		n = new JTextField();
		z = new JTextField();
		k = new JTextField();
		w = new JTextField();
		b = new Board();
		
		JButton symuluj = new JButton("Symuluj");
		symuluj.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e){
				int mm = 0;
				int nn = 0;
				int zz = 0;
				int kk = 0;
				int ww = 0;
				try{
					mm = Integer.parseInt(m.getText());
					nn = Integer.parseInt(n.getText());
					zz = Integer.parseInt(z.getText());
					kk = Integer.parseInt(k.getText());
					ww = Integer.parseInt(w.getText());
				}
				catch (NumberFormatException r){}
				
				if(mm < 1 || nn < 1 || zz <1 || kk < 1 || ww < 1 || zz+ww > mm*nn){
					JOptionPane.showMessageDialog(b, "Niepoprawne dane wejsciowe. Sprobuj ponownie.","Blad",JOptionPane.ERROR_MESSAGE);
					m.setText("");
					n.setText("");
					z.setText("");
					k.setText("");
					w.setText("");
				}
				else{
					b.init(mm,nn,zz,kk,ww);
					b.repaint();
				}
			}
		});
		
		JButton help = new JButton("Pomoc");
		help.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e){
				JOptionPane.showMessageDialog(b,"Szerokosc planszy - liczba naturalna, minimalnie 1\n"+
				"Wysokosc planszy - liczba naturalna, minimalnie 1\n"+
				"Liczba zajacow - liczba naturalna, minimalnie 1\n"+
				"Opoznienie w milisekundach - liczba naturalna, minimalnie 1\n\n"+
				"Suma liczby zajacow i wilkow nie moze przekroczyc liczby pol na planszy", "Pomoc", JOptionPane.PLAIN_MESSAGE);
			}
		});
		
		p.add(help);
		p.add(mLabel);
		p.add(m);
		p.add(nLabel);
		p.add(n);
		p.add(kLabel);
		p.add(k);
		p.add(zLabel);
		p.add(z);	
		p.add(wLabel);
		p.add(w);
		p.add(new JLabel());
		p.add(symuluj);
			
		f.add("North", p);
		f.add("Center", b);
		f.setVisible(true);
	}
}