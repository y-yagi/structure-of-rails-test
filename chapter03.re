= テストの為の機能たち

前章では、Railsが提供しているテスト用のクラスについて説明しました。Railsではそれらクラス以外にもテスト実行する為の機能が提供されています。本章では、これらテストの為の仕組みについて照会します。

== Fixtures

Fixturesはテスト用のデータを作る為の機能です。基本的にはテストで使用する事を想定しています。

テスト用のデータといえばfactory_botが有名です。factory_botが独自DSLでデータを定義するのに対して、FixturesではYAMLでデータの定義を行います。

//list[users.yml][users.yml]{
taro:
  email: taro@example.com
  name: Taro

hanako:
  email: hanako@example.com
  name: Hanako
//}

RailsでYAMLというと、ERBの記法を使用出来るようにしている事が多いのですが、Fixturesも例に漏れずそうです。YAMLとしてパース処理を行う前に、ERBで前処理が行われます。そのため、ファイル内でERBの記法を使用出来ます。

//list[yml_with_erb][YAML内でERB記法を使用]{
<% 100.times do |n| %>
user_<%= n %>:
  email: <%= "user#{n}@example.com" %>
  name: <%= "user#{n}" %>
<% end %>
//}

=== 処理の流れ

Fixturesは下記のような流れで処理を行います。

 1. "test/fixtures"配下のYAMLファイルをロード
 2. YAMLファイルから対象のテーブル名を取得&データを組立
 3. 対象のテーブルから登録されているデータを削除&データの登録

まず"test/fixtures"配下にある拡張子が"yml"になっているファイルがまとめて読み込まれます。これはサブディレクトリ配下のファイルも全て読み込まれます。

次に、YAMLファイルから対象のテーブル名を取得&データの組立を行います。テーブル名はデフォルトだとファイル名から取得されます。例えば"users.yml"というファイルがあった場合、"users"というテーブルに対してSQLを実行します。サブディレクトリ配下については、そのサブディレクトリ名が指定された状態でテーブル名が組み立てられます("admin/users.yml"というファイルがあった場合、テーブル名は"admin_users"になる)。

最後に登録されているデータの削除&データの登録を行います。データの削除を行う事で、仮に既にデータが登録されていても、必ずFixtureに定義されたデータのみが登録されている状態にします。この削除&登録は全てのテーブルに対する処理が1つのSQLでまとめて実行されるようになっています。

=== トランザクショナルテスト

Fixturesでは、テスト実行前にトランザクションを開始&テスト終了時にロールバックを実行し、テスト内で作成したデータがデータベースに残らないようにしています。これにより、テスト毎に毎回Fixtureの削除&登録処理をしないでも、同じ状態でテストが実施出来るようにしています。

この挙動は@<code>{use_transactional_tests}で変更可能で、@<code>{use_transactional_tests}にfalseを指定すると、トランザクションを使用する代わりに、テスト毎にデータの削除&登録を行うようになります。

これはテストを実行するスレッドが1つの場合問題無いのですが、スレッドが複数あって、それぞれ違うデータベースのコネクションを所有した場合問題になります。例えば、Capybaraを使用してテストを行う場合、アプリケーションサーバとCapybaraが起動するスレッドが別々になる為、この問題が発生します。

Railsでは、この問題に対応する為に異なるスレッドが同じデータベースのコネクションを使用する機能実装し、Fixturesを使用する場合のみその機能を有効化するようにしています。

=== File fixtures

新規にRailsアプリケーションを作成すると、"test/fixtures"ディレクトリ配下に"files"というディレクトリが合わせて作成されます。このディレクトリにはテストで使用するファイル(テキストファイル、動画、PDF等々)を格納する為のディレクトリです。

このディレクトリに格納されたファイルは、@<code>{file_fixture}というメソッドで簡単に参照出来るようになります。

//list[file_fixture][file_fixture]{
file_fixture("example.txt") #=> Pathnameクラスのインスタンス
file_fixture("example.mp3").size
//}

名前が似ているのですが、これはここまで説明してきたFixturesとは別の単純にファイルを参照する為の仕組みです。"test/fixtures/files"はYAMLではなくただのファイルを格納する想定のディレクトリである、という事だけご認識頂ければと思います。

== Test Runner

Test Runnerはテストを実行する為の機能です。RSpecを使用している場合"rspec"スクリプトを使用してテストを実行しているかと思います。minitestにはこの"rspec"スクリプト相当のものがなく@<fn>{minitest}、rubyかrakeを使用してテストを実行していました。しかしこれだと、特定の行のテストを実行する、複数のテストをまとめて実行する、等が実施しづらい、という問題がありました。
//footnote[minitest][minitest 5系までの話です。6系だと実行用のスクリプトを追加する予定があるらしいです。]

それらの問題を解決する為に、Railsでは独自にテストを実行する為の仕組みをRails 5.0から導入しました。これにより、上記のような任意のテストの実行や、失敗したテストについて再実行するコマンド等の表示が出来るようになりました。これがTest Runnerと呼ばれている機能です。Test Runnerは、"rails test"コマンドで実行出来ます。

//list[rails test][rails test]{
$ ./bin/rails test test/models/user_test.rb:5
Run options: --seed 1833

# Running:

.

Finished in 2.243887s, 0.4457 runs/s, 0.4457 assertions/s.
1 runs, 1 assertions, 0 failures, 0 errors, 0 skips

$ ./bin/rails test test/models/receive_mail_test.rb
Run options: --seed 41533

# Running:

F

Failure:
ReceiveMailTest#test_validation [test/models/receive_mail_test.rb:5]:
Expected false to be truthy.


bin/rails test test/models/receive_mail_test.rb:4



Finished in 2.110546s, 0.4738 runs/s, 0.4738 assertions/s.
1 runs, 1 assertions, 1 failures, 0 errors, 0 skips
//}

Test Runnerは他にもテスト結果に色をつけたり、失敗したら即座にテストを終了する為の機能が組込まれています。


=== minitest plugin

minitestは本体の機能が大分少ないのですが、代わりに拡張機能(plugin)を追加する為の機能がデフォルトで提供されています。

Test Runnerは、何回か作り直された結果、現在はminitestの拡張機能として実装されています。Test Runnerのコードを見たい方はminitestの拡張機能の使い方を把握しておくと読みやすいと思います。minitestの拡張機能については、xxをご参照下さい。

== Parallel Testing

名前の通りテストを並列で実行する為の仕組みです。Rails 6.0の目玉機能の1つ。

@<code>{parallelize}というメソッドが提供されており、ユーザはそのメソッドを使用するだけでテストが並列で実行されるようになります。

//list[parallelize][parallelize]{
class ActiveSupport::TestCase
  parallelize(workers: :number_of_processors)
end
//}

テストを実行するワーカーは@<code>{workers}引数で指定可能です。@<code>{workers}には数字を基本的には数字をしするのですが、@<code>{:number_of_processors}というSymbolを指定した場合、自動でマシンのコア数の値を指定してくれます。

並列化は、デフォルトではプロセスをforkして行われます。forkが使えない環境では、代わりにスレッドを使用する事が出来ます。スレッドを使用したい場合は、@<code>{with}引数に、@<code>{:threads}と指定すればOKです

//list[parallelize][parallelize]{
class ActiveSupport::TestCase
  parallelize(workers: :number_of_processors, with: :threads)
end
//}

並列化にプロセスを指定した場合とスレッドを指定した場合とで挙動が違います。プロセスの場合、プロセスをfork後データベースを新規に作成、及び、schemaのロードを行い、テストではその新規に作成したデータベースを使用するようになります。そのため、プロセス毎に違うデータベースを使用するようになり、プロセス間で処理が競合しないようになっています。プロセス間でのデータのやりとりには、dRuby@<fn>{druby}が使われるようになっています。なお、作成されたデータベースはテストの後処理で削除されるようになっています。
//footnote[druby][@<href>{https://docs.ruby-lang.org/ja/latest/library/drb.html}]

スレッドの場合、執筆時点ではこのデータベースの作成は行われず、全てのスレッドで同じデータベースに対して処理を行うようになっています。テストの内容によってはスレッドの排他制御を意識する必要がありますので注意してください。

なお、minitestには元々parallel executorという並列でテストをする仕組みがあり、Parallel Testingはその仕組みに依存しています。スレッドの場合の処理はminitestのparallel executorをそのまま使用しているだけです。そのため、他のテストフレームワークでそのまま使用出来るような機能ではありませんのでご注意下さい。
