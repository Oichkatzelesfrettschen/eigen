#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <string_view>

// This function takes a line that may contain a name and/or email address
// and returns just the name, while fixing the "bad cases".
std::string contributor_name(std::string_view line) {
    std::string result;

    // let's first take care of the case of isolated email addresses, like
    // "user@localhost.localdomain" entries
    if (line.find("markb@localhost.localdomain") != std::string_view::npos) {
        return "Mark Borgerding";
    }

    if (line.find("kayhman@contact.intra.cea.fr") != std::string_view::npos) {
        return "Guillaume Saupin";
    }

    // from there on we assume that we have a entry of the form
    // either:
    //   Bla bli Blurp
    // or:
    //   Bla bli Blurp <bblurp@email.com>

    std::size_t position_of_email_address = line.find_first_of('<');
    if (position_of_email_address != std::string_view::npos) {
        // there is an e-mail address in <...>.

        // Hauke once committed as "John Smith", fix that.
        if (line.find("hauke.heibel") != std::string_view::npos)
            result = "Hauke Heibel";
        else {
            // just remove the e-mail address
            result = line.substr(0, position_of_email_address);
        }
    } else {
        // there is no e-mail address in <...>.

        if (line.find("convert-repo") != std::string_view::npos)
            result = "";
        else
            result = line;
    }

    // remove trailing spaces
    size_t length = result.length();
    while (length >= 1 && result[length - 1] == ' ')
        result.erase(--length);

    return result;
}

// parses hg churn output to generate a contributors map.
std::map<std::string, int> contributors_map_from_churn_output(const char *filename) {
    std::map<std::string, int> contributors_map;

    std::string line;
    std::ifstream churn_out;
    churn_out.open(filename, std::ios::in);
    while (std::getline(churn_out, line)) {
        // remove the histograms "******" that hg churn may draw at the end of some lines
        std::size_t first_star = line.find_first_of('*');
        if (first_star != std::string::npos)
            line.erase(first_star);

        // remove trailing spaces
        std::size_t length = line.length();
        while (length >= 1 && line[length - 1] == ' ')
            line.erase(--length);

        // now the last space indicates where the number starts
        std::size_t last_space = line.find_last_of(' ');

        // get the number (of changesets or of modified lines for each contributor)
        int number;
        std::istringstream(line.substr(last_space + 1)) >> number;

        // get the name of the contributor
        line.erase(last_space);
        std::string name = contributor_name(line);

        auto it = contributors_map.find(name);
        // if new contributor, insert
        if (it == contributors_map.end())
            contributors_map.emplace(name, number);
        // if duplicate, just add the number
        else
            it->second += number;
    }
    churn_out.close();

    return contributors_map;
}

// find the last name, i.e. the last word.
// for "van den Schbling" types of last names, that's not a problem, that's actually what we want.
std::string lastname(const std::string &name) {
    std::size_t last_space = name.find_last_of(' ');
    if (last_space >= name.length() - 1)
        return name;
    else
        return name.substr(last_space + 1);
}

struct contributor {
    std::string name;
    int changedlines;
    int changesets;
    std::string url;
    std::string misc;

    contributor() : changedlines(0), changesets(0) {}

    bool operator<(const contributor &other) {
        return lastname(name).compare(lastname(other.name)) < 0;
    }
};

void add_online_info_into_contributors_list(std::list<contributor> &contributors_list,
                                            const char *filename) {
    std::string line;
    std::ifstream online_info;
    online_info.open(filename, std::ios::in);
    while (std::getline(online_info, line)) {
        std::string hgname, realname, url, misc;

        std::size_t last_bar = line.find_last_of('|');
        if (last_bar == std::string::npos)
            continue;
        if (last_bar < line.length())
            misc = line.substr(last_bar + 1);
        line.erase(last_bar);

        last_bar = line.find_last_of('|');
        if (last_bar == std::string::npos)
            continue;
        if (last_bar < line.length())
            url = line.substr(last_bar + 1);
        line.erase(last_bar);

        last_bar = line.find_last_of('|');
        if (last_bar == std::string::npos)
            continue;
        if (last_bar < line.length())
            realname = line.substr(last_bar + 1);
        line.erase(last_bar);

        hgname = line;

        // remove the example line
        if (hgname.find("MercurialName") != std::string::npos)
            continue;

        auto it = std::find_if(contributors_list.begin(), contributors_list.end(),
                               [&](const contributor &c) { return c.name == hgname; });

        if (it == contributors_list.end()) {
            contributor c;
            c.name = realname;
            c.url = url;
            c.misc = misc;
            contributors_list.push_back(c);
        } else {
            it->name = realname;
            it->url = url;
            it->misc = misc;
        }
    }
}

int main() {
    // parse the hg churn output files
    std::map<std::string, int> contributors_map_for_changedlines =
        contributors_map_from_churn_output("churn-changedlines.out");
    // map<string,int> contributors_map_for_changesets =
    // contributors_map_from_churn_output("churn-changesets.out");

    // merge into the contributors list
    std::list<contributor> contributors_list;
    for (const auto &[name, lines] : contributors_map_for_changedlines) {
        contributor c;
        c.name = name;
        c.changedlines = lines;
        c.changesets = 0; // contributors_map_for_changesets.find(it->first)->second;
        contributors_list.push_back(c);
    }

    add_online_info_into_contributors_list(contributors_list, "online-info.out");

    contributors_list.sort();

    cout << "{| cellpadding=\"5\"\n";
    cout << "!\n";
    cout << "! Lines changed\n";
    cout << "!\n";

    int i = 0;
    for (const auto &c : contributors_list) {
        if (c.name.length() == 0)
            continue;
        if (i % 2)
            std::cout << "|-\n";
        else
            std::cout << "|- style=\"background:#FFFFD0\"\n";
        if (c.url.length())
            std::cout << "| [" << c.url << " " << c.name << "]\n";
        else
            std::cout << "| " << c.name << "\n";
        if (c.changedlines)
            std::cout << "| " << c.changedlines << "\n";
        else
            std::cout << "| (no information)\n";
        std::cout << "| " << c.misc << "\n";
        i++;
    }
    std::cout << "|}" << std::endl;
}
