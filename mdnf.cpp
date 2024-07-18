#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <set>

#include <SFML/Graphics.hpp>

int table_size = 2;
int start = 0;
std::map<std::string, std::vector<std::string>> table;
std::map<std::string, std::vector<int>> table_steps;
std::vector<std::string> masks;
std::vector<std::string> table_keys;
std::vector<std::string> function;
std::map<std::string, std::set<std::string>> table_absorbtion;
std::set<std::string> absolute_answers;
std::vector<int> answer_rows;
std::set<std::set<std::string>> enumerated_answers;
std::vector<std::vector<std::string>> final_answer;

bool TableKeysCmp(std::string first_string, std::string second_string) {
    if (first_string.size() != second_string.size()) return first_string.size() < second_string.size();
    else return first_string < second_string;
}

std::set<std::string> Substrings(std::string key) {
    std::set<std::string> substrings;
    for (int i = 0; i < (1 << key.size()); ++i) {
        std::string substring = "";
        for (int j = 0; j < key.size(); ++j) {
            if (i & (1 << j)) substring += key[j];
        }
        if (substring.size() > 0) substrings.insert(substring);
    }

    return substrings;
}

bool IsFunctionCorrect() {
    bool ans = false;
    for (auto el : function) if (std::stoi(el)) ans = true;
    return ans;
}

void MaskGenerator() {
    for (int i = 0; i < (1 << table_size); ++i) {
        std::string temp = "";
        for (int j = table_size - 1; j >= 0; --j) {
            temp += std::to_string((i >> j) & 1);
        }
        masks.push_back(temp);
    }
}

void KeysGenerator() {
    std::string chars = "";
    for (int i = 'a'; i < 'a' + table_size; ++i) chars += (char)i;
    for (int i = 0; i < (1 << table_size); ++i) {
        std::string temp = "";
        for (int j = 0; j < table_size; ++j) {
            if (masks[i][j] == '1') temp += chars[j];
        }
        table_keys.push_back(temp);
    }

    std::sort(table_keys.begin(), table_keys.end(), TableKeysCmp);
    table_keys.erase(table_keys.begin());
}

void TableAggregator() {
    for (int i = 0; i < table_size; ++i) {
        std::vector<std::string> table_values;
        for (int j = 0; j < (1 << table_size); ++j) {
            std::string temp = "";
            table_values.push_back(temp + masks[j][i]);
        }
        table[table_keys[i]] = table_values;
    }
    for (int i = table_size; i < (1 << table_size); ++i) {
        std::vector<std::string> table_values;
        for (int j = 0; j < (1 << table_size); ++j) {
            std::string value = "";
            for (int k = 0; k < table_keys[i].size(); ++k) {
                std::string temp = "";
                value += table[temp + table_keys[i][k]][j];
            }
            table_values.push_back(value);
        }
        table[table_keys[i]] = table_values;
    }
    table.erase(table.begin());

    return;
}

void TableStepsAggregator() {
    for (auto key : table_keys) {
        std::vector<int> table_values(1 << table_size, 0);
        table_steps[key] = table_values;
    }

    return;
}

void FunctionAggregator() {
    std::string func = "";
    for (int i = 0; i < (1 << table_size); ++i) func += "0";
    if (func.size() == (1 << table_size)) {
        for (auto el : func) {
            std::string temp = "";
            function.push_back(temp + el);
        }
    }
    return;
}

void FirstStep() {
    for (int i = 0; i < (1 << table_size); ++i) {
        if (function[i] == "0") {
            for (auto key : table_keys) {
                table_steps[key][i] = 1;
            }
        }
    }

    return;
}

void SecondStep() {
    for (auto key : table_keys) {
        std::vector<std::string> black_list;
        for (int i = 0; i < (1 << table_size); ++i) {
            if (table_steps[key][i] == 1) black_list.push_back(table[key][i]);
        }
        for (int i = 0; i < (1 << table_size); ++i) {
            if (table_steps[key][i] == 0) {
                for (auto c : black_list) if (table[key][i] == c) table_steps[key][i] = 2;
            }
        }
    }

    return;
}

void TableAbsorbtionAggregator() {
    for (auto key : table_keys) {
        for (auto substring : Substrings(key)) {
            if (substring != key) table_absorbtion[substring].insert(key);
        }
    }
    return;
}

void ThirdStep() {
    for (auto key : table_keys) {
        for (int i = 0; i < (1 << table_size); ++i) {
            if (table_steps[key][i] == 0) {
                for (auto el : table_absorbtion[key]) {
                    table_steps[el][i] = 3;
                }
            }
        }
    }

    return;
}

void Exception() {
    std::vector<std::pair<std::string, std::string>> absolute_columns;
    std::vector<int> absolute_rows;

    // first (fourth) step
    for (int i = 0; i < (1 << table_size); ++i) {
        int counter = 0;
        std::string absolute_key;
        for (auto key : table_keys) {
            if (table_steps[key][i] == 0) {
                ++counter;
                absolute_key = key;
            }
        }
        if (counter == 1) {
            std::string absolute_ans = "";
            for (int j = 0; j < absolute_key.size(); ++j) {
                if (table[absolute_key][i][j] == '0') absolute_ans += "!";
                absolute_ans += absolute_key[j];
            }
            absolute_answers.insert(absolute_ans);
            table_steps[absolute_key][i] = 4;
            absolute_columns.push_back({absolute_key, table[absolute_key][i]});
        }
    }

    // second (fifth) step
    for (auto el : absolute_columns) {
        for (int i = 0; i < (1 << table_size); ++i) {
            if (table_steps[el.first][i] == 0 && table[el.first][i] == el.second) {
                table_steps[el.first][i] = 5;
                absolute_rows.push_back(i);
            }
        }
    }
//    for (auto el : absolute_columns) {
//        for (int i = 0; i < (1 << table_size); ++i) {
//            if (table_steps[el.first][i] == 0 && table[el.first][i] != el.second) {
//                std::string absolute_ans = "";
//                for (int j = 0; j < el.first.size(); ++j) {
//                    if (table[el.first][i][j] == '0') absolute_ans += "!";
//                    absolute_ans += el.first[j];
//                }
//                absolute_answers.insert(absolute_ans);
//                table_steps[el.first][i] = 5;
//                absolute_rows.push_back(i);
//            }
//        }
//    }
//
    // third (sixth) step
    for (auto row : absolute_rows) {
        for (auto key : table_keys) {
            if (table_steps[key][row] == 0) table_steps[key][row] = 6;
        }
    }

    // fourh (seventh) step
    for (int i = 0; i < (1 << table_size); ++i) {
        for (auto key : table_keys) {
            if (table_steps[key][i] == 0) {
                answer_rows.push_back(i);
                break;
            }
        }
    }

    return;
}

std::set<std::set<std::string>> Enumeration(int row) {
    int row_number = answer_rows.size() - 1 - row;
    std::set<std::set<std::string>> answer, previous_answer;
    if  (row == 0) {
        for (auto key : table_keys) {
            if (table_steps[key][answer_rows[row_number]] == 0) {
                std::string ans = "";
                for (int j = 0; j < key.size(); ++j) {
                    if (table[key][answer_rows[row_number]][j] == '0') ans += "!";
                    ans += key[j];
                }
                answer.insert({ans});
            }
        }
    } else {
        previous_answer = Enumeration(row - 1);
        for (auto key : table_keys) {
            if (table_steps[key][answer_rows[row_number]] == 0) {
                if (previous_answer.size() == 0) {
                    std::string ans = "";
                    for (int i = 0; i < key.size(); ++i) {
                        if (table[key][answer_rows[row_number]][i] == '0') ans += "!";
                        ans += key[i];
                    }
                    answer.insert({ans});
                } else {
                    for (auto it : previous_answer) {
                        std::string ans = "";
                        for (int i = 0; i < key.size(); ++i) {
                            if (table[key][answer_rows[row_number]][i] == '0') ans += "!";
                            ans += key[i];
                        }
                        it.insert({ans});
                        answer.insert(it);
                    }
                }
            }
        }
    }

    return answer;
}

int MDNFSize(std::set<std::string> mdnf) {
    int size = 0;
    for (auto el : mdnf) {
        for (auto letter : el) {
            if (letter != '!') ++size;
        }
    }
    return size;
}

void MDNF() {
    int min_size = -1;
    for (auto answer : enumerated_answers) {
        if (min_size == -1 || MDNFSize(answer) < min_size) {
            min_size = MDNFSize(answer);
            final_answer.clear();
        }
        if (MDNFSize(answer) == min_size) {
            std::vector<std::string> final_ans;
            for (auto ans : absolute_answers) final_ans.push_back(ans);
            for (auto ans : answer) final_ans.push_back(ans);
            final_answer.push_back(final_ans);
        }
    }

    return;
}

void Backend() {
    masks.clear();
    table_keys.clear();
    table.clear();
    table_steps.clear();
    MaskGenerator();
    KeysGenerator();
    TableAggregator();
    if (start == 0) {
        function.clear();
        FunctionAggregator();
        return;
    }
    TableStepsAggregator();
    FirstStep();
    SecondStep();
    TableAbsorbtionAggregator();
    ThirdStep();
    Exception();
    if  (start == 1) return;
    if (!answer_rows.size() && absolute_answers.size()) {
        std::vector<std::string> temp;
        for (auto el : absolute_answers) temp.push_back(el);
        final_answer.push_back(temp);
        return;
    }
    if (!answer_rows.size() && !absolute_answers.size()) return;
    enumerated_answers = Enumeration(answer_rows.size() - 1);
    MDNF();

//    std::cout << std::setw(table_size + 1) << "func : ";
//    for (auto el : function) std::cout << std::setw(table_size + 1) << el << " ";
//    std::cout << "\n";
//    for (auto key : table_keys) {
//        std::cout << std::setw(table_size + 1) << key << " : ";
//        for (auto val : table[key]) std::cout << std::setw(table_size + 1) << val << " ";
//        std::cout << "\n";
//    }
//    for (auto key : table_keys) {
//        std::cout << std::setw(table_size + 1) << key << " : ";
//        for (auto val : table_steps[key]) std::cout << std::setw(table_size + 1) << val << " ";
//        std::cout << "\n";
//    }
//    for (auto answer : enumerated_answers) {
//        for (auto ans : answer) std::cout << ans << " ";
//        std::cout << "\n";
//    }
//    std::cout << enumerated_answers.size() << "\n";
//    for (auto answer : final_answer) {
//        for (auto ans : answer) std::cout << ans << " ";
//        std::cout << "\n";
//    }

    return;
}


const int width = 1550, height = 870;
int current_step = 0;
int row_size;

sf::RectangleShape TableBorder(float x_coor, float y_coor, float wid, float hei) {
    sf::RectangleShape table_border;
    table_border.setFillColor(sf::Color::Black);
    table_border.setPosition(x_coor, y_coor);
    table_border.setSize(sf::Vector2f(wid, hei));

    return table_border;
}

std::string TableRow(int row_number) {
    std::string row = "";
    for (auto key : table_keys) {
        row += " ";
        row += table[key][row_number];
    }
    row += " ";
    row += function[row_number];
    return row;
}

sf::RectangleShape LineDelete(int row_number, float wid) {
    sf::RectangleShape line_delete;
    line_delete.setSize(sf::Vector2f(wid, 2));
    line_delete.setPosition(5, (float)((row_number + 1) * 26) + 13);
    line_delete.setFillColor(sf::Color::Red);

    return line_delete;
}

sf::RectangleShape CellDelete(std::string key, int row_number, int step_number) {
    float x_coordinate = 3;
    for (int i = 0; i < table_keys.size(); ++i) {
        if (table_keys[i] == key) break;
        x_coordinate += ((float)table_keys[i].size() + 1) * 12;
    }
    sf::RectangleShape cell_delete;
    cell_delete.setSize(sf::Vector2f(((float)key.size() + 1) * 12, 2));
    cell_delete.setPosition(x_coordinate, (float)((row_number + 1) * 26) + 13);
    if (step_number == 2) cell_delete.setFillColor(sf::Color::Blue);
    else if (step_number == 3) cell_delete.setFillColor(sf::Color::Magenta);

    return cell_delete;
}

sf::RectangleShape DrawingExceptions(std::string key, int row_number, int step_number) {
    float x_coordinate = 3;
    for (int i = 0; i < table_keys.size(); ++i) {
        if (table_keys[i] == key) break;
        x_coordinate += ((float)table_keys[i].size() + 1) * 12;
    }
    sf::RectangleShape exception;
    exception.setSize(sf::Vector2f(((float)key.size() + 1) * 12, 26));
    exception.setPosition(x_coordinate, (float)((row_number + 1) * 26));
    if (step_number == 4) exception.setFillColor(sf::Color::Green);
    else if (step_number == 5 || step_number == 6) exception.setFillColor(sf::Color::Yellow);

    return exception;
}

sf::RectangleShape Button(float x_coor, float y_coor, float wid, float hei) {
    sf::RectangleShape button;
    button.setSize(sf::Vector2f(wid, hei));
    button.setPosition(sf::Vector2f(x_coor, y_coor));
    button.setOutlineColor(sf::Color::Black);
    button.setOutlineThickness(2);
    button.setFillColor(sf::Color{255, 255, 224});

    return button;
}

sf::RectangleShape ButtonSign(float x_coor, float y_coor, float wid, float hei, int orientation) {
    if (!orientation) {
        x_coor += 2;
        y_coor = (y_coor * 2 + hei) / 2 - 1;
        wid -= 4;
        hei = 2;
    } else {
        y_coor += 2;
        x_coor = (x_coor * 2 + wid) / 2 - 1;
        hei -= 4;
        wid = 2;
    }

    sf::RectangleShape sign;
    sign.setSize(sf::Vector2f(wid, hei));
    sign.setPosition(x_coor, y_coor);
    sign.setFillColor(sf::Color::Black);

    return sign;
}

sf::RectangleShape FunctionCell(int row_number) {
    float x_coordinate = 3;
    for (int i = 0; i < table_keys.size(); ++i) {
        x_coordinate += ((float)table_keys[i].size() + 1) * 12;
    }
    sf::RectangleShape function_cell;
    function_cell.setSize(sf::Vector2f(24, 26));
    function_cell.setPosition(x_coordinate, (float)((row_number + 1) * 26));

    return function_cell;
}

void ShowResult() {
    sf::RenderWindow result(sf::VideoMode(width, height), "MDNF");

    sf::Font font;
    font.loadFromFile("droid-sans-mono.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);
    text.setPosition(10, 10);
    std::string results;
    for (auto answer : final_answer) {
        for (auto ans : answer) {
            results += ans;
            if (ans != answer.back()) results += " | ";
            else results += "\n";
        }
    }
    text.setString(results);

    while (result.isOpen()) {
        sf::Event event;

        while (result.pollEvent(event)) {
            // closing
            if (event.type == sf::Event::Closed) {
                result.close();
                return;
            }
        }

        result.clear(sf::Color{255, 239, 213});

        result.draw(text);

        result.display();
    }
}

void Frontend() {
    sf::RenderWindow window(sf::VideoMode(width, height), "MDNF");

    sf::Font font;
    font.loadFromFile("droid-sans-mono.ttf");
    sf::Text table_row;
    table_row.setFillColor(sf::Color::Black);
    table_row.setCharacterSize(20);
    table_row.setFont(font);
    table_row.setPosition(0, 3);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);


    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            // closing window
            if (event.type == sf::Event::Closed) window.close();

            // click
            if (event.type == sf::Event::MouseButtonPressed) {
                auto [mouse_x, mouse_y] = sf::Mouse::getPosition(window);
                if (start == 0) {
                    if (Button(width - 125, 70, 100, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        ++start;
                    }
                    if (Button(width - 60, 20, 30, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        if (table_size >= 2 && table_size < 5) ++table_size;
                    } else if (Button(width - 120, 20, 30, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        if (table_size > 2 && table_size <= 5) --table_size;
                    }
                    Backend();
                } else if (start == 1) {
                    if (Button(width - 125, 20, 100, 30).getGlobalBounds().contains(mouse_x, mouse_y)  &&
                        IsFunctionCorrect()) {
                        ++start;
                        Backend();
                    }
                    for (int i = 0; i < function.size(); ++i) {
                        if (FunctionCell(i).getGlobalBounds().contains(mouse_x, mouse_y))
                            function[i] = std::to_string(!(bool)std::stoi(function[i]));
                    }
                } else if (start == 2) {
                    if (Button(width - 125, 20, 100, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        if (current_step < 6) ++current_step;
                        else ++start;
                    } else if (Button(width - 125, 70, 100, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        if (current_step > 0) --current_step;
                    }
                    std::cout << current_step << "\n";
                } else if (start == 3) {
                    if (Button(width - 125, 20, 100, 30).getGlobalBounds().contains(mouse_x, mouse_y)) {
                        ShowResult();
                    }
                }
            }
        }

        std::string table_title = "";
        for (auto key : table_keys) {
            table_title += " ";
            table_title += key;
        }
        table_title += " F";
        table_row.setString(table_title);
        table_row.setPosition(0, 3);

        row_size = TableRow(0).size();

        window.clear(sf::Color{255, 239, 213});

        // plus, minus and set size
        if (start == 0) {
            window.draw(Button(width - 60, 20, 30, 30));
            window.draw(ButtonSign(width - 60, 20, 30, 30, 0));
            window.draw(ButtonSign(width - 60, 20, 30, 30, 1));
            window.draw(Button(width - 120, 20, 30, 30));
            window.draw(ButtonSign(width - 120, 20, 30, 30, 0));

            window.draw(Button(width - 125, 70, 100,30));
            text.setPosition(width - 123, 70);
            text.setString("SET SIZE");
            window.draw(text);
        }

        // set function
        if (start == 1) {
            window.draw(Button(width - 125, 20, 100,30));
            text.setPosition(width - 123, 20);
            text.setString("SET FUNC");
            window.draw(text);
        }

        // steps change
        if (start == 2) {
            window.draw(Button(width - 125, 20, 100,30));
            text.setPosition(width - 100, 20);
            text.setString("NEXT");
            window.draw(text);
            window.draw(Button(width - 125, 70, 100,30));
            text.setPosition(width - 123, 70);
            text.setString("PREVIOUS");
            window.draw(text);
        }

        // show result
        if (start == 3) {
            window.draw(Button(width - 125, 20, 100,30));
            text.setPosition(width - 110, 20);
            text.setString("RESULT");
            window.draw(text);
        }


        // steps drawing
        for (int k = 0; k <= current_step; ++k) {
            if (k >= 1) {
                for (int i = 0; i < function.size(); ++i) {
                    if (function[i] == "0") window.draw(LineDelete(i, (float) row_size * 12));
                }
            }
            if (k >= 2) {
                for (auto key : table_keys) {
                    for (int i = 0; i < (1 << table_size); ++i) {
                        if (table_steps[key][i] == 2)
                            window.draw(CellDelete(key, i, 2));
                        else if (table_steps[key][i] == 3 && k >= 3)
                            window.draw(CellDelete(key, i, 3));
                        else if (table_steps[key][i] == 4 && k >= 4)
                            window.draw(DrawingExceptions(key, i, 4));
                        else if (table_steps[key][i] == 5 && k == 5)
                            window.draw(DrawingExceptions(key, i, 5));
                        else if (table_steps[key][i] == 6 && k == 6)
                            window.draw(DrawingExceptions(key, i, 6));
                    }
                }
            }
        }



        // table drawing
        window.draw(table_row);
        for (int i = 0; i < (1 << table_size); ++i) {
            table_row.setPosition(0, (float)((i + 1) * 26) + 3);
            table_row.setString(TableRow(i));
            window.draw(table_row);
        }
        for (int i = 0; i <= (1 << table_size) + 1; ++i) {
            window.draw(TableBorder(3, (float)(i * 26), (float)(row_size * 12), 2));
        }
        float x_coordinate = 3;
        float hei = (((float)(1 << table_size)) + 1) * 26 + 2;
        window.draw(TableBorder(x_coordinate, 0, 2, hei));
        for (int i = 0; i < table_keys.size(); ++i) {
            x_coordinate += ((float)table_keys[i].size() + 1) * 12;
            window.draw(TableBorder(x_coordinate, 0, 2, hei));
        }
        window.draw(TableBorder(x_coordinate + 24, 0, 2, hei));

        window.display();
    }

    return;
}


int main() {
    Backend();
    Frontend();

    return 0;
}