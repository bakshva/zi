#include "liba.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>

long long p;
int n, N, H, T, opened;
std::vector<long long> c, d, code, deck, tbl;
std::vector<std::vector<long long>> hand;
std::unordered_map<long long, std::string> names;

Fl_Int_Input *in_n, *in_N, *in_H, *in_T;
Fl_Input *in_p;
Fl_Hold_Browser *players;
Fl_Box *table_box, *hand_box;
Fl_Text_Display *log_view;
Fl_Text_Buffer *log_buf;

long long rnd(long long m) {
  return (long long)(((((unsigned long long)rand()) << 31) |
                      (unsigned long long)rand()) %
                     (unsigned long long)m);
}

void say(const std::string &s) {
  log_buf->append(s.c_str());
  log_view->insert_position(log_buf->length());
  log_view->show_insert_position();
}

void refresh(Fl_Widget * = nullptr, void * = nullptr) {
  std::string s = "Стол:";
  for (int t = 0; t < T; t++)
    s += " " + (t < opened ? names[tbl[t]] : std::string("[?]"));
  table_box->copy_label(s.c_str());

  int i = players->value() - 1;
  if (i < 0 || i >= (int)hand.size()) {
    s = "Выберите игрока в списке";
  } else {
    s = "Карты игрока " + std::to_string(i + 1) + ":";
    for (long long v : hand[i])
      s += " " + names[v];
  }
  hand_box->copy_label(s.c_str());
}

void deal_cb(Fl_Widget *, void *) {
  int new_n = atoi(in_n->value());
  int new_N = atoi(in_N->value());
  int new_H = atoi(in_H->value());
  int new_T = atoi(in_T->value());

  if (new_n < 2 || new_H < 1 || new_T < 0 || new_n * new_H + new_T > new_N) {
    say("неверные параметры: нужно игроков >= 2, на руки >= 1, "
        "игроков * на руки + на стол <= карт в колоде\n");
    return;
  }
  n = new_n;
  N = new_N;
  H = new_H;
  T = new_T;

  log_buf->text("");

  long long pin = 0;
  std::string line = in_p->value();
  if (!line.empty()) {
    std::stringstream ss(line);
    ss >> pin;
  }

  if (pin > (long long)N * N && pin < (1LL << 62) && pin % 2 == 1 &&
      ferma(pin) && ferma((pin - 1) / 2)) {
    p = pin;
  } else {
    if (!line.empty()) {
      say("p должно быть вида 2q + 1 (p, q простые) и N^2 < p < 2^62, "
          "генерирую\n");
    }
    long long q;
    do {
      q = ((1LL << 60) + rnd(1LL << 60)) | 1;
      p = 2 * q + 1;
    } while (!ferma(q) || !ferma(p));
  }
  say("p = " + std::to_string(p) + ", q = " + std::to_string((p - 1) / 2) +
      "\n");

  c.assign(n, 0);
  d.assign(n, 0);
  for (int j = 0; j < n; j++) {
    while (true) {
      long long a = 2 + rnd(p - 3);
      long long b = p - 1;
      auto pr = gcd(false, &a, &b);
      if (pr.first == 1) {
        c[j] = a;
        d[j] = ((pr.second % (p - 1)) + (p - 1)) % (p - 1);
        break;
      }
    }
  }

  const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8",
                         "9", "10", "В", "Д", "К", "Т"};
  const char *suits[] = {" пик", " черв", " бубен", " треф"};
  code.assign(N, 0);
  names.clear();
  for (int k = 0; k < N; k++) {
    code[k] = (long long)(k + 1) * (k + 1);
    names[code[k]] = k < 52 ? std::string(ranks[k % 13]) + suits[k / 13]
                            : "№" + std::to_string(k + 1);
  }

  deck = code;
  for (int j = 0; j < n; j++) {
    for (auto &x : deck) {
      x = pows(x, c[j], p);
    }
    for (int i = N - 1; i > 0; i--) {
      std::swap(deck[i], deck[rnd(i + 1)]);
    }
    say("шаг 1: игрок " + std::to_string(j + 1) +
        " зашифровал колоду и перемешал\n");
  }

  hand.assign(n, {});
  for (int k = 0; k < H; k++) {
    for (int i = 0; i < n; i++) {
      long long v = deck[k * n + i];
      std::string s = "карта " + std::to_string(k * n + i + 1) +
                      " игроку " + std::to_string(i + 1) + ": " +
                      std::to_string(v);
      for (int j = 0; j < n; j++) {
        if (j != i) {
          v = pows(v, d[j], p);
          s += " -> " + std::to_string(v);
        }
      }
      say(s + "\n");
      hand[i].push_back(pows(v, d[i], p));
    }
  }

  tbl.assign(T, 0);
  opened = 0;

  players->clear();
  for (int i = 0; i < n; i++) {
    players->add(("Игрок " + std::to_string(i + 1)).c_str());
  }
  refresh();
}

void open_cb(Fl_Widget *, void *) {
  if (deck.empty() || opened >= T)
    return;

  int m = (opened == 0) ? std::min(3, T) : 1;
  for (int t = opened; t < opened + m; t++) {
    long long v = deck[H * n + t];
    std::string s = "карта " + std::to_string(H * n + t + 1) +
                    " на стол: " + std::to_string(v);
    for (int j = 0; j < n; j++) {
      v = pows(v, d[j], p);
      s += " -> " + std::to_string(v);
    }
    say(s + "\n");
    tbl[t] = v;
  }
  opened += m;
  refresh();
}

int main() {
  std::srand((unsigned)std::time(nullptr));

  Fl_Double_Window win(1000, 720, "Ментальный покер");

  in_n = new Fl_Int_Input(10, 30, 120, 28, "Игроков:");
  in_n->align(FL_ALIGN_TOP_LEFT);
  in_n->value("4");

  in_N = new Fl_Int_Input(150, 30, 140, 28, "Карт в колоде:");
  in_N->align(FL_ALIGN_TOP_LEFT);
  in_N->value("52");

  in_H = new Fl_Int_Input(310, 30, 120, 28, "Карт на руки:");
  in_H->align(FL_ALIGN_TOP_LEFT);
  in_H->value("2");

  in_T = new Fl_Int_Input(450, 30, 120, 28, "Карт на стол:");
  in_T->align(FL_ALIGN_TOP_LEFT);
  in_T->value("5");

  in_p = new Fl_Input(590, 30, 280, 28, "p (пусто - сгенерировать):");
  in_p->align(FL_ALIGN_TOP_LEFT);

  Fl_Button *deal = new Fl_Button(880, 30, 110, 28, "Раздать");
  deal->callback(deal_cb);

  players = new Fl_Hold_Browser(10, 95, 170, 250, "Игроки");
  players->align(FL_ALIGN_TOP_LEFT);
  players->callback(refresh);

  table_box = new Fl_Box(200, 95, 790, 60);
  table_box->box(FL_BORDER_BOX);
  table_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  table_box->labelsize(20);

  hand_box = new Fl_Box(200, 165, 790, 60);
  hand_box->box(FL_BORDER_BOX);
  hand_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  hand_box->labelsize(20);

  Fl_Button *open = new Fl_Button(200, 240, 250, 40, "Открыть карты стола");
  open->callback(open_cb);

  log_view = new Fl_Text_Display(10, 375, 980, 335, "Журнал протокола");
  log_view->align(FL_ALIGN_TOP_LEFT);
  log_buf = new Fl_Text_Buffer();
  log_view->buffer(log_buf);

  win.end();
  win.show();

  refresh();

  return Fl::run();
}