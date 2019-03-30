= Railsのテスト

まずはじめに、Railsが提供しているテストの概要について説明します。

== テストフレームワーク

Rubyでテストフレームワークといえば、test-unit、minitest、RSpecの名前が挙がることが多いと思います。Railsは、かつてtest-unitを使っていて、現在はminitestを使用しています。Rails自体のテストも、Railsを使用するユーザに向けたテストの仕組みも、どちらもminitestを使用しています。日本でRailsアプリケーションの開発をされている場合、RSpecをお使いのケースが多いのではないかと思います。しかしRSpecはRailsがデフォルトでサポートしているテストフレームワークではなく、Rails自体にRSpecの為の機能は全くありません。

== minitestとRails

先に述べた通り、Railsではminitestを使用しています。が、minitestをそのまま使用している訳ではありません。色々と機能拡張を行っています。

例えば、minitestでは一致しない事をテストしたい場合、@<code>{refute}というメソッド、及び、@<code>{refute}で始まるメソッドを使用します。例えば値が一致しない事を確認したい場合、@<code>{refute_equal}というメソッドを使用します。

//list[refute_equal][refute_equal]{
refute_equal 5, User.count
//}

しかしRailsでは、@<code>{refute}を使うことを推奨していません。代わりに、@<code>{assert_not}というメソッド、及び、@<code>{assert_not}で始まるメソッドが提供されており、そちらを使用することを推奨しています。先の例だと、@<code>{assert_not_equal}を使用する必要があります。
　
//list[refute_equal][assert_not_equal]{
assert_not_equal 5, User.count
//}

元々test-unitには@<code>{assert_not}があり、test-unitからminitestに移行する際に互換性の為にこれらのメソッドが追加されました。

その後、@<code>{refute}を使用するようにする? という提案もあったようなのですが、それは進まず、@<code>{assert_not}を使う形のままで落ち着いています@<fn>{refute}。
//footnote[refute][当時の議論が全ては見つからなかったので推測混じりなのですが、@<code>{refute}という名前をあまり好ましく思わない人がいたため、移行は行われなかったようです。恐らくレーサー。]

なお、@<code>{refute}ではなく@<code>{assert_not}を使用する事をチェックする為のRuboCopのcop@<fn>{AssertNot}があります。Railsのリポジトリではこのcopが有効化されており、@<code>{refute}は一切使われないようになっています。
//footnote[AssertNot][@<href>{https://www.rubydoc.info/gems/rubocop/RuboCop/Cop/Rails/RefuteMethods}]

他にも、Railsではテストを書く際、@<code>{test}というメソッドを使用してテストを定義します。

//list[test][testメソッド]{
test "should get index" do
  get users_url
  assert_response :success
end
//}

これもRailsが提供しているメソッドです。minitestを使用している場合このメソッドは使用する事は出来ません。

そのため、「Railsはmintiestを使用している」というより、「Railsはminitestを拡張した独自の仕組みを使用している」というのが正確な状態になっています。

== テスト用のクラス

Railsは機能ごとにライブラリがわかれています。例えばO/RマッパーのActive Record、メール送信のAction Mailer、バックグランドジョブのActive Job、という具合です。

Railsでは、各ライブラリ毎に、そのライブラリの機能を提供する為のクラスを提供しています。クラス図は次のようになっています。

//image[classtree][クラス図]{
//}

紙面の都合上大分クラスを割愛しています。minitestのテスト用のクラスであるMinitest::Testを継承したActiveSupport::TestCaseというクラスがあり、各ライブラリのテストクラスはそのActiveSupport::TestCaseを継承している、という点だけ覚えておいて下さい。各クラスの詳細については次章で説明します。


== まとめ

本章では、Railsが提供しているテストの概要について説明しました。Railsはminitestを使用している、しかしそのまま使用しているのではなく、minitestを拡張して使用している、という点だけご理解頂けると幸いです。
