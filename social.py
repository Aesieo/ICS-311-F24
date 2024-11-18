from collections import defaultdict
from datetime import datetime
from wordcloud import WordCloud
import matplotlib.pyplot as plt

class Comment:
    def __init__(self, content, user, timestamp):
        self.content = content
        self.user = user
        self.timestamp = timestamp

class Post:
    def __init__(self, content, user, timestamp):
        self.content = content
        self.user = user
        self.timestamp = timestamp
        self.comments = []
        self.viewers = []

    def add_comment(self, comment):
        self.comments.append(comment)

    def add_viewer(self, viewer):
        if viewer not in self.viewers:
            self.viewers.append(viewer)

class User:
    def __init__(self, username, attributes = None):
        self.username = username
        self.attributes = attributes or {}
        self.uploads = []
        self.seen = []
        self.comments = []
        self.connections = {}

    def upload(self, post):
        self.uploads.append(post)

    def view(self, post):
        self.seen.append(post)
        post.add_viewer(self.username)

    def comment(self, post, comment):
        comment = Comment(content = comment, user = self.username, timestamp = datetime.now())
        self.comments.append(comment)
        post.add_comment(comment)

    def connection(self, user, con_type):
        self.connections[user] = con_type

def keyword(posts):
    keyword_map = defaultdict(list)
    for post in posts:
        raw_words = post.content.split()
        for word in set(raw_words):
            keyword_map[word.lower()].append(post)

def generate_word_cloud(posts, users, include=None, exclude=None, attributes=None):
    filtered_posts = []
    for post in posts:
        if include and not any(kw in post.content for kw in include):
            continue
        if exclude and any(kw in post.content for kw in exclude):
            continue
        if attributes:
            uploader = next((user for user in users if user.username == post.user), None)
            if not uploader:
                continue
            if 'age' in attributes and attributes['age'] != uploader.attributes['age']:
                continue
            if 'gender' in attributes and attributes['gender'] != uploader.attributes['gender']:
                continue
            if 'country' in attributes and attributes['country'] != uploader.attributes['country']:
                continue
            if 'job' in attributes and attributes['job'] != uploader.attributes['job']:
                continue
        filtered_posts.append(post)

    raw_text = " ".join(post.content for post in filtered_posts)

    wordcloud = WordCloud(width = 1000, height = 500, background_color = "white").generate(raw_text)
    plt.figure(figsize=(10,5))
    plt.imshow(wordcloud, interpolation="bilinear")
    plt.axis("off")
    plt.show()

def main():
    # Sample text
    text1 = "Pizza, a dish that has transcended cultures and borders, is often regarded as a universal favorite. Originating from Italy, its classic form features a round, flat base of leavened wheat-based dough topped with tomatoes, cheese, and various toppings. The versatility of pizza allows it to cater to diverse palates, with options ranging from the traditional Margherita to more adventurous combinations like barbecue chicken or vegan alternatives. The social aspect of sharing a pizza has played a significant role in its global popularity, making it a staple at gatherings, parties, and casual dining experiences. As pizza continues to evolve, local adaptations and gourmet interpretations reflect the culinary creativity of different regions, solidifying its status not just as food, but as a cultural phenomenon that brings people together."

    # Users created
    Mike = User("Mike", attributes={"age":"21", "gender":"male", "country":"Italy", "job":"Pizza Chef"})
    Vic = User("Vic", attributes={"age":"18", "gender":"female", "country":"Italy", "job":"Cashier"})
    PizzaHater321 = User("PizzaHater321", attributes={"age":"12", "gender":"male", "country":"USA", "job":"Student"})

    # Connections created
    Mike.connection(Vic.username, "coworker")
    Vic.connection(Mike.username, "coworker")
    PizzaHater321.connection(Mike.username, "seen")
    PizzaHater321.connection(Vic.username, "seen")

    # Post 1, post content, comments, views
    post1 = Post(content = text1, user = Mike.username, timestamp = datetime.now())
    Mike.upload(post1)
    Vic.view(post1)
    PizzaHater321.view(post1)
    Vic.comment(post1, "See you at work")
    PizzaHater321.comment(post1, "Nah")

    # Post 2, post content, comments, views
    post2 = Post(content = "Torus Shaped Pizza", user = Vic.username, timestamp = datetime.now())
    Vic.upload(post2)
    PizzaHater321.view(post2)
    Mike.view(post2)
    PizzaHater321.comment(post2, "Ew")
    Mike.comment(post2, "Mama mia what are you doing with our stock")
    Vic.comment(post2, "It's called innovation bozo")
    Mike.comment(post2, "We're going bankrupt")

    # Post 3, post content, comments, views
    post3 = Post(content = "I Hate Pizza", user = PizzaHater321.username, timestamp = datetime.now())
    PizzaHater321.upload(post1)

    # Post and User list to use for generating word cloud
    posts = [post1, post2, post3]
    users = [Mike, Vic, PizzaHater321]


    generate_word_cloud(
        posts,
        users,
        include=None,
        exclude=None,
        attributes={'country':"Italy"}
    )

if __name__ == "__main__":
    main()