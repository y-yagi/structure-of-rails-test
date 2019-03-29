= テストの為の機能

前章では、Railsが提供しているテスト用のクラスについて説明しました。Railsではそれらクラス以外にもテスト実行する為の仕組みが提供されています。本章では、これらテストの為の仕組みについて照会します。

==
Fixtures

== Test Runner

Test Runnerはテストを実行する為の機能です。RSpecを使用している場合"rspec"スクリプトを使用してテストを実行しているかと思います。minitestにはこの"rspec"スクリプト相当のものがなく@<fn>{minitest}、rubyかrakeを使用してテストを実行していました。しかしこれだと、特定の行のテストを実行する、複数のテストをまとめて実行する、等が実施しづらい、という問題がありました。
//footnote[minitest][minitest 5系までの話です。6系だとスクリプトを追加する予定があるらしいです。]

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

minitestは本体の機能が大分少ないのですが、代わりに拡張機能を追加する為の機能がデフォルトで提供されています。Test Runnerは(何回か作り直された結果)その機能を使用して、minitestの拡張機能(プラグイン)として実装されています。Test Runnerのコードを見たい方はminitestの拡張機能の使い方を把握しておくと読みやすいと思います。

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

なお、minitestには元々parallel executorという並列でテストをする仕組みがあり、Parallel Testingはその仕組みに依存しています。スレッドの場合の処理はminitestのparallel executorをそのまま使用しているだけです。そのため、他のテストフレームワークでそのまま使用出来るような機能ではありません。
